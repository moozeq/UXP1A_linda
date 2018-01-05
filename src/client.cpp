#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <signal.h>
#include "Request.h"
#include "Reply.h"
#include "CommandParser.h"
#include <semaphore.h>
#include <ctime>

using namespace std;
string pipePath = "/tmp/fifo.";
const char* serverPath = "/tmp/fifo.server";
const char * inFifoSemaphoreName = "/serverInFifoSemaphore";

void sig_handler(int signo) {
	if(signo == SIGINT){
		unlink(pipePath.c_str());
	cout<<"Pipe's been unlinked"<<endl;
	}
}

void createInPipe(pid_t clientPid) {
	pipePath.append(to_string(clientPid));
	umask(0);
	mkfifo(pipePath.c_str(), 0666);
	cout<<"FIFO's been created at "<<pipePath<<endl;
}

void sendRequest(ofstream * outStream, sem_t * semaphore, const Request * req)
{
	if (sem_wait(semaphore) < 0)
		perror("sem_wait(3) failed on child");
	(*outStream)<<(*req);
	if (sem_post(semaphore) < 0)
		perror("sem_post(3) error on child");
}

int init(sem_t ** serverFifoSem) {
	pid_t clientPid = getpid();
	cout<<"Client's starting..."<<endl<<"Client's PID: "<<clientPid<<endl;
	if(access(serverPath, F_OK) == -1) {
		cout<<"Server doesn't exist"<<endl;
		return 1;
	}

	// Open server fifo semaphore
	(*serverFifoSem) = sem_open(inFifoSemaphoreName, O_RDWR);
	if (*serverFifoSem == SEM_FAILED)
	{
		perror("sem_open(3) failed");
		exit(EXIT_FAILURE);
	}
	createInPipe(clientPid);
	signal(SIGINT, sig_handler);
	return 0;
}

int main() {
	sem_t * serverFifoSemaphore;
	if (init(&serverFifoSemaphore) != 0)
		return 0;

	ofstream outFIFO(serverPath, ofstream::binary);


	// Get new Request from user
	bool exit = false;
	while(!exit) {
		Request *req = new Request();
		bool correct = false;
		do {
			CommandParser::showOptions();
			string line;
			getline(cin, line);
			exit = CommandParser::checkIfExit(line);
			if (exit)
				break;
			correct = CommandParser::parseCommand(line, req);
		} while (!correct);

		if (exit) {
			delete req;
			break;
		}
		sendRequest(&outFIFO, serverFifoSemaphore,req);
		cout<<"Request for tuple has been sent"<<endl;
		cout<<*req;
		delete req;

		// Get reply from server
		Reply* rep = new Reply();
		ifstream inFIFO(pipePath.c_str(), ifstream::binary);
		ofstream outClientTmpFifo(pipePath, ofstream::binary);
		cout<<"Waiting for reply"<<endl;
		inFIFO >> *rep;

		cout<<"Reply: "<<std::endl;
		cout<<*(rep->tuple);
		delete rep;
	}
	unlink(pipePath.c_str());
	if (sem_close(serverFifoSemaphore) < 0)
	        perror("sem_close(3) failed");
	cout<<"Client" << getpid() << " exited" << endl;
	return 0;
}

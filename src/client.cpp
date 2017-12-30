#include <sys/types.h>
#include <sys/stat.h>
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

#include <ctime>

#define BUFFSIZE 256

using namespace std;
string pipePath = "/tmp/fifo.";
const char* serverPath = "/tmp/fifo.server";

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

int init() {
	pid_t clientPid = getpid();
	cout<<"Client's starting..."<<endl<<"Client's PID: "<<clientPid<<endl;
		if(access(serverPath, F_OK) == -1) {
			cout<<"Server doesn't exist"<<endl;
			return 1;
		}
	createInPipe(clientPid);
	signal(SIGINT, sig_handler);
	return 0;
}

int main() {
	if (init() != 0)
		return 0;

	ofstream outFIFO(serverPath, ofstream::binary);

	// Get time and convert to string
	std::time_t t = std::time(nullptr);
	std::string tmpString;
	char mbstr[100];
	std::strftime(mbstr, sizeof(mbstr), "%A %c", std::localtime(&t));
	tmpString = mbstr;
	tmpString = tmpString.substr(tmpString.find(":")-2, 8);

	// Set up new Request and send to server
	Request *req = new Request();
	Tuple * tup = new Tuple({{true, tmpString}, {false, std::to_string(getpid())}});
	req->procId = getpid();
	req->reqType = Request::Read;
	req->timeout = 1;
	req->setTuple(tup);

	outFIFO << *req;
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
	unlink(pipePath.c_str());

	delete rep;
	return 0;
}

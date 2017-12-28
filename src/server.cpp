#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <signal.h>
#include <pthread.h>
#include <fstream>
#include <unordered_set>
#include "Request.h"
#include "Reply.h"
#include "GuardedQueue.h"

#define BUFFSIZE 256

using namespace std;
const char* serverPath = "/tmp/fifo.server";
GuardedQueue writeReqQueue;
GuardedQueue readReqQueue;
bool childrenActiveFlag;

std::unordered_set<Tuple> tupleSpace;	// Global tuple space

void sig_handler(int signo) {
	if(signo == SIGINT) {
		unlink(serverPath);
		cout<<"Server's pipe's been unlinked"<<endl;
	}
}

void sigHandlerExit(int signo)
{
	pthread_exit(NULL);
}

void createServerPipe(const char* serverPath) {
	umask(0);
	mkfifo(serverPath, 0666);
	cout<<"Server's FIFO's been created at ";
	cout<<serverPath<<endl;
}

int init() {
	pid_t serverPid = getpid();
	cout<<"Server's starting..."<<endl<<"Server's PID: "<<serverPid<<endl;

	if(access(serverPath, F_OK) != -1) {
		cout<<"Server's already existed, exiting...";
		return 1;
	}
	signal(SIGINT, sig_handler);
	signal(SIGUSR1, sigHandlerExit);
	createServerPipe(serverPath);
	return 0;
}

/**
 * 	@brief	Finds tuple in tuple space and returns reply.
 */
Reply* search(const Tuple* reqTup, unsigned opType) {
	Reply* reply = new Reply();
	unordered_set<Tuple>::iterator tupleIter = tupleSpace.find(*reqTup);
	if(tupleIter != tupleSpace.end())
	{
		reply->isFound = true;
		Tuple * foundTupleCopy = new Tuple(*tupleIter);
		reply->setTuple(foundTupleCopy);
	}
	else
	{
		reply->isFound = false;
	}
	return reply;
}

void* service(void) {
	Request* req = readReqQueue.consumerEnter();
	cout<<"New Request found in server readReqQueue: "<<endl;
	cout<<"Request from: " << req->procId <<endl;
	cout<<"Request type: ";
	switch(req->reqType) {
		case Request::Input:
			cout<<"input";break;
		case Request::Output:
			cout<<"output";break;
		case Request::Read:
			cout<<"read";break;
	}
	cout<<endl<<"Requested tuple: "<<endl;
	cout<<*(req->tuple)<<endl;
	//Reply* reply = search(req->tuple, req->reqType);
	Reply * reply = new Reply();
	Tuple * tuple = new Tuple(*(req->tuple));
	reply->setTuple(tuple);
	reply->isFound = true;

	string clientFIFO = "/tmp/fifo.";
	clientFIFO.append(to_string(req->procId));
	ofstream outFIFO(clientFIFO.c_str(), ofstream::binary);

	outFIFO << *reply;
	cout<<"Reply to client"<<req->procId <<", has been sent"<<endl;
	delete reply;
	delete req;
	return 0;
}

void * receptionistThread(void * iStream)
{
	ifstream * inFifo = static_cast<ifstream *>(iStream);

	while(1)
	{
		Request *req = new Request();
		*inFifo >> *req;
		if(req->reqType == Request::Output)
			writeReqQueue.producerEnter(req);
		else
			readReqQueue.producerEnter(req);
		cout<<"New request received in server receptionist thread..."<<endl;
	}
	return 0;
}

int main() {
	if (init() != 0)
		return 0;

	// Tuple Space test data
	Tuple tup{{true, std::string("tekst")}, {false, std::string("23")}};
	tupleSpace.insert(tup);
	tupleSpace.insert({{true, std::string("krotka")},
		{true, std::string("testowa")},{false, std::string("3")}});

	ifstream inFIFO(serverPath, ifstream::binary);

	pthread_t recThread;
	pthread_create(&recThread, NULL, &receptionistThread, static_cast<void *>(&inFIFO));

	service();
	pthread_kill(recThread, SIGUSR1);
	unlink(serverPath);
	cout<<"Server's pipe's been unlinked"<<endl;

	return 0;
}

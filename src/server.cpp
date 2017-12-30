#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
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
#include <semaphore.h>

#define BUFFSIZE 256

using namespace std;
const char* serverPath = "/tmp/fifo.server";
const char * inFifoSemaphoreName = "/serverInFifoSemaphore";
GuardedQueue writeReqQueue;
GuardedQueue readReqQueue;
Request * receptThreadReqPtr;
sem_t * inputFifoSemaphore;

std::unordered_set<Tuple> tupleSpace;	// Global tuple space

void sig_handler(int signo) {
	if(signo == SIGINT) {
		unlink(serverPath);
		cout<<"Server's pipe's been unlinked"<<endl;
	}
}

void sigHandlerExit(int signo)
{
	if(receptThreadReqPtr != nullptr)
		delete receptThreadReqPtr;
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

	// Initialize input fifo semaphore
	inputFifoSemaphore = sem_open(
			inFifoSemaphoreName, O_CREAT, (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP), 1);
	if (inputFifoSemaphore == SEM_FAILED)
	{
	        perror("sem_open(3) error");
	        exit(EXIT_FAILURE);
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

void* service(void *) {
	while(1)
	{
		cout<<"service __ Request* req = readReqQueue.consumerEnter();"<<endl;
		Request* req = readReqQueue.consumerEnter();
		cout<<"New Request found in server readReqQueue - popped from queue: "<<endl;
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
	}
	return 0;
}

void * receptionistThread(void * iStream)
{
	ifstream * inFifo = static_cast<ifstream *>(iStream);

	while(1)
	{
		receptThreadReqPtr = new Request();
		*inFifo >> *receptThreadReqPtr;
		if(receptThreadReqPtr->reqType == Request::Output)
			writeReqQueue.producerEnter(receptThreadReqPtr);
		else
			readReqQueue.producerEnter(receptThreadReqPtr);
		cout<<"New request received in server receptionist thread..."<<endl;
		cout<<*(receptThreadReqPtr->tuple)<<endl;
	}
	return 0;
}

int main() {
	if (init() != 0)
		return 0;

	ifstream inFIFO(serverPath, ifstream::binary);
	ofstream outServerTmpFifo(serverPath, ofstream::binary);

	pthread_t recThread, readerThread;
	pthread_create(&recThread, NULL, &receptionistThread, static_cast<void *>(&inFIFO));
	pthread_create(&readerThread, NULL, &service, NULL);

	sleep(5);
	cout<<"Sleep(5) ended"<<endl;
	pthread_kill(recThread, SIGUSR1);
	pthread_kill(readerThread, SIGUSR1);
	cout<<"Pthread kill sent"<<endl;
	unlink(serverPath);
	cout<<"Server's pipe's been unlinked (main)"<<endl;

	// Close and unlink semaphore
	if (sem_close(inputFifoSemaphore) < 0) {
		perror("sem_close(3) failed");
		sem_unlink(inFifoSemaphoreName);
		exit(EXIT_FAILURE);
	}
	if (sem_unlink(inFifoSemaphoreName) < 0)
	        perror("sem_unlink(3) failed");
	return 0;
}

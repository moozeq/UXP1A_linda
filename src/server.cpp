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
//#include <unordered_set>
#include <map>
#include "Request.h"
#include "Reply.h"
#include "GuardedQueue.h"
#include <semaphore.h>

using namespace std;
const char* serverPath = "/tmp/fifo.server";
const char * inFifoSemaphoreName = "/serverInFifoSemaphore";
GuardedQueue writeReqQueue;
GuardedQueue readReqQueue;
sem_t * inputFifoSemaphore;
multimap<size_t, Tuple> tupleSpace; //Global tuple space
bool stopThreadsFlag;

void sig_handler(int signo) {
	if(signo == SIGINT) {
		unlink(serverPath);
		cout<<"Server's pipe's been unlinked"<<endl;
	}
}

void createServerPipe(const char* serverPath) {
	umask(0);
	mkfifo(serverPath, 0666);
	cout<<"Server's FIFO's been created at ";
	cout<<serverPath<<endl;
}

void sendRequest(ofstream * outStream, sem_t * semaphore, const Request * req)
{
	if (sem_wait(semaphore) < 0)
		perror("sem_wait(3) failed on child");
	(*outStream)<<(*req);
	if (sem_post(semaphore) < 0)
		perror("sem_post(3) error on child");
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
			inFifoSemaphoreName, O_CREAT | O_RDWR, (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP), 1);
	if (inputFifoSemaphore == SEM_FAILED)
	{
	        perror("sem_open(3) error");
	        exit(EXIT_FAILURE);
	}

	stopThreadsFlag = false;
	signal(SIGINT, sig_handler);
	createServerPipe(serverPath);
	return 0;
}
/*
 * return:
 * 0 - no sign (==)
 * 1 - >
 * 2 - >=
 * 3 - <
 * 4 - <=
 * 5 - *
 */
int checkIfSign(string pattern) {
	if (pattern.size() > 1) {
		if (pattern[0] == '>' && !(pattern[1] == '='))
			return 1;
		else if (pattern[0] == '>' && pattern[1] == '=')
			return 2;
		else if (pattern[0] == '<' && !(pattern[1] == '='))
			return 3;
		else if (pattern[0] == '<' && pattern[1] == '=')
			return 4;
	}
	if (pattern.size() == 1 && pattern[0] == '*')
		return 5;
	return 0; //only 1 char, cannot be sign
}

bool checkPattern(Elem el1, Elem el2) {
	if(el1.isString != el2.isString)
		return false;
	if(el1.isString) {
		return true; //to complete
	}
	else { //integer
		int sign = checkIfSign(el2.pattern);
		int i; //offset in string
		switch (sign) {
			case 0: i = 0; break;
			case 1:
			case 3: i = 1; break;
			case 2:
			case 4: i = 2; break;
			case 5: return true; //* - can be anything
		}
		int first, second;
		try {
			first = stoi(el1.pattern);
			second = stoi(el2.pattern.substr(i));
		}
		catch (invalid_argument& e) {
			return false;
		}
		switch (sign) {
			case 0: return first == second;
			case 1: return first > second;
			case 3: return first >= second;
			case 2: return first < second;
			case 4: return first <= second;
		}
	}
	return false;
}

/**
 * 	@brief	Finds tuple in tuple space and returns reply.
 */
Reply* search(const Tuple* reqTup, unsigned opType) {
	Reply* reply = new Reply();
	pair <multimap<size_t, Tuple>::iterator, multimap<size_t, Tuple>::iterator> ret;
	const size_t tupHash = reqTup->getHash();
	ret = tupleSpace.equal_range(tupHash);
	for (multimap<size_t, Tuple>::iterator it = ret.first; it != ret.second; ++it)
	{
		unsigned i;
		for (i = 0; i < reqTup->elems.size(); ++i) {
			if (!checkPattern(it->second.elems[i], reqTup->elems[i]))
				break;
		}
		if (i != reqTup->elems.size())
			continue;
		reply->isFound = true;
		reply->setTuple(new Tuple(it->second));
		return reply;
	}
	reply->isFound = false;
	return reply;
}

void* service(void *) {
	while(!stopThreadsFlag)
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
			case Request::Stop:
			{
				cout<<"Request::Stop received in service() thread"<<endl;
				delete req;
				return 0;
			}
		}
		cout<<endl<<"Requested tuple: "<<endl;
		cout<<*(req->tuple)<<endl;
		Reply * reply = search(req->tuple, req->reqType);

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

	while(!stopThreadsFlag)
	{
		Request * incomingReq = new Request();
		*inFifo >> *incomingReq;
		if(incomingReq->reqType == Request::Output)
			writeReqQueue.producerEnter(incomingReq);
		else if(incomingReq->reqType == Request::Stop)
		{
			Request * doubledReq = new Request(*incomingReq);
			readReqQueue.producerEnter(doubledReq);
			writeReqQueue.producerEnter(incomingReq);
			return 0;
		}
		else
			readReqQueue.producerEnter(incomingReq);
		cout<<"New request received in server receptionist thread..."<<endl;
		cout<<*(incomingReq->tuple)<<endl;
	}
	return 0;
}

void addTestSet() {
	Tuple* tup = new Tuple();
	string st1 = "std";
	string st2 = "3";
	Elem* el1 = new Elem(true, st1);
	Elem* el2 = new Elem(false, st2);
	tup->elems.push_back(*el1);
	tup->elems.push_back(*el2);
	tupleSpace.insert(make_pair(tup->getHash(), *tup));
	delete tup;
	delete el1;
	delete el2;
}

int main() {
	if (init() != 0)
		return 0;

	addTestSet();
	ifstream inFIFO(serverPath, ifstream::binary);
	ofstream outServerTmpFifo(serverPath, ofstream::binary);

	pthread_t recThread, readerThread;
	pthread_create(&recThread, NULL, &receptionistThread, static_cast<void *>(&inFIFO));
	pthread_create(&readerThread, NULL, &service, NULL);

	sleep(360);

	// Stop children threads - send Stop request
	stopThreadsFlag = true;
	Request * r = new Request();
	r->procId = getpid();
	r->reqType = Request::Stop;
	r->timeout = 713;
	sendRequest(&outServerTmpFifo, inputFifoSemaphore, r);
	delete r;

	// Join children threads
	pthread_join(recThread, NULL);
	pthread_join(readerThread, NULL);
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

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


#define BUFFSIZE 256

using namespace std;
const char* serverPath = "/tmp/fifo.server";

std::unordered_set<Tuple> tupleSpace;

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

int init() {
	pid_t serverPid = getpid();
	cout<<"Server's starting..."<<endl<<"Server's PID: "<<serverPid<<endl;

	if(access(serverPath, F_OK) != -1) {
		cout<<"Server's already existed, exiting...";
		return 1;
	}
	signal(SIGINT, sig_handler);
	createServerPipe(serverPath);
	return 0;
}

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

void* service(void* oReq) {
	Request* req = (Request*)oReq;
	cout<<"request from: " << req->procId <<endl;
	cout<<"request type: ";
	switch(req->reqType) {
		case Request::Input:
			cout<<"input";break;
		case Request::Output:
			cout<<"output";break;
		case Request::Read:
			cout<<"read";break;
	}
	cout<<endl<<"request tuple: ";
	for (unsigned i = 0; i < req->tuple->elems.size(); ++i) {
		cout<< req->tuple->elems[i].pattern << " ";
	}
	cout << endl;
	Reply* reply = search(req->tuple, req->reqType);

	string clientFIFO = "/tmp/fifo.";
	clientFIFO.append(to_string(req->procId));
	ofstream outFIFO(clientFIFO.c_str(), ofstream::binary);

	outFIFO << *reply;
	cout<<"Reply to client"<<req->procId <<", has been sent"<<endl;
	delete reply;
	return 0;
}

int main() {
	if (init() != 0)
		return 0;

	// Tuple Space test
	Tuple tup{{true, std::string("tekst")}, {false, std::string("23")}};
	tupleSpace.insert(tup);
	tupleSpace.insert({{true, std::string("krotka")},
		{true, std::string("testowa")},{false, std::string("3")}});

	ifstream inFIFO(serverPath, ifstream::binary);

	Request *req = new Request();

	inFIFO >> *req;
	cout<<"Request sent: "<<endl;
	//pthread_t thr;
	//pthread_create(&thr, NULL, &service, (void*)req);
	service((void*)req);
	unlink(serverPath);
	cout<<"Server's pipe's been unlinked"<<endl;

	delete req;
	return 0;
}

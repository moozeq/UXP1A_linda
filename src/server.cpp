/*
 * main.cpp
 *
 *  Created on: Dec 15, 2017
 *      Author: piotr
 */
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

#include "linda.h"
#define BUFFSIZE 256
using namespace std;
const char* serverPath = "/tmp/fifo.server";

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

void* service(void* req) {
	Request* inReq = (Request*)req;
	cout<<"request from: " << inReq->procId <<endl;
	cout<<"request tuple: "<< inReq->tuple->elems[0].pattern;
	return 0;
}

int main() {
	if (init() != 0)
		return 0;

	std::ifstream inFIFO(serverPath, ifstream::binary);

	Request *req = new Request();

	inFIFO >> *req;
	cout<<"Request transfered: "<<endl;
	pthread_t thr;
	pthread_create(&thr, NULL, &service, (void*)req);

	unlink(serverPath);
	cout<<"Server's pipe's been unlinked"<<endl;
	return 0;
}

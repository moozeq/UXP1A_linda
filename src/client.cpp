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
#include "linda.h"
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

	string str = "hello";
	Request *req = new Request();
	Elem el = Elem(true, str);
	req->procId = getpid();
	req->reqType = 0;
	req->timeout = 1;
	req->tuple->elems.push_back(el);

	outFIFO << *req;
	cout<<"Tuple's been sent"<<endl;

	Reply* rep = new Reply();
	ifstream inFIFO(pipePath.c_str(), ifstream::binary);
	cout<<"Waiting for reply"<<endl;
	inFIFO >> *rep;

	cout<<"Reply: "<<rep->tuple->elems[0].pattern<<endl;
	unlink(pipePath.c_str());

	delete rep;
	delete req;
	return 0;
}

#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <signal.h>
#include "linda.h"

using namespace std;

void sig_handler(int signo) {
	if(signo == SIGINT)
		exit(1);
}

int main() {

	signal(SIGINT, sig_handler);
	// Get new Request from user
	bool exit = false;
	while(true) {
		Request *req = new Request();
		bool correct = false;
		do {
			CommandParser::showOptions();
			string line;
			getline(cin, line);
			exit = CommandParser::checkIfExit(line);
			if (exit)
				break;
			CommandParser::changeToDefSendRcvMessage(line);
			correct = CommandParser::parseCommand(line, req);
		} while (!correct);

		if (exit) {
			delete req;
			break;
		}
		switch (req->reqType) {
		case Request::Input:
			linda_input(*(req->tuple), req->timeout); break;
		case Request::Output:
			linda_output(*(req->tuple)); break;
		case Request::Read:
			linda_read(*(req->tuple), req->timeout); break;
		}

		delete req;
	}
	cout<<"Client" << getpid() << " exited" << endl;
	return 0;
}

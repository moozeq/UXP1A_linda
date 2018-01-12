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
			linda_input(req); break;
		case Request::Output:
			linda_output(req); break;
		case Request::Read:
			linda_read(req); break;
		default:
			break;
		}

		delete req;
	}

	// Another usage
	linda_output({{true, std::string("drugaKrotka")},
		{true, std::string("testowa")},{false, std::string("19")}, {true, std::string("koniec")}});

	// Another usage
	linda_output(Tuple({{true, string("prostaKrotka")}}));

	cout<<"Client" << getpid() << " exited" << endl;
	return 0;
}

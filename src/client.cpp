#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <signal.h>
#include "linda.h"
#include "GuardedCout.h"

using namespace std;

const std::string commonClientsLogFileName("ClientsCommon.log");

void sig_handler(int signo) {
	if(signo == SIGINT)
		exit(1);
}

int main(int argc, char * argv[]) {
	bool showCommandsInTerminal = true;
	if(argc > 1 && strcmp(argv[1], "-nocommands") == 0)
		showCommandsInTerminal = false;

	signal(SIGINT, sig_handler);

	GuardedCout gCout("ClientsCoutSemaphoreName");
	// Get new Request from user
	bool exit = false;
	while(true) {
		Request *req = new Request();
		bool correct = false;
		do {
			if(showCommandsInTerminal)
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
		case Request::Input:{
			gCout.print("Input request has been sent with timeout: ", req->timeout, "\n");
			gCout.print("Tuple:\n");
			gCout.print(*(req->tuple), "\n");
			Reply rep = linda_input(req);
			if(rep.isFound){
				gCout.print("Requested tuple popped from tuple space\n");
				gCout.print(*(rep.tuple), "\n");
			}
			else
				gCout.print("Requested tuple not found...\n");
			break;
		}
		case Request::Output:{
			gCout.print("Output request has been sent\n");
			gCout.print("Tuple:\n");
			gCout.print(*(req->tuple), "\n");
			linda_output(req);
			break;
		}
		case Request::Read:{
			gCout.print("Read request has been sent with timeout: ", req->timeout, "\n");
			gCout.print("Tuple:\n");
			gCout.print(*(req->tuple), "\n");
			Reply rep = linda_read(req);
			if(rep.isFound){
				gCout.print("Requested tuple read from tuple space:\n");
				gCout.print(*(rep.tuple), "\n");
			}
			else
				gCout.print("Requested tuple not found...\n");
			break;
		}
		default:
			break;
		}

		delete req;
	}

	gCout.print("Client ", getpid(), " exited\n");
	return 0;
}

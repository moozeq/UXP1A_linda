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
		case Request::Input:{
			cout<<"Input request has been sent with timeout: "<< req->timeout <<endl;
			cout<<"Tuple: "<<endl;
			cout<<*(req->tuple)<<endl;
			Reply rep = linda_input(req);
			if(rep.isFound){
				cout<<"Requested tuple popped from tuple space:"<<endl;
				cout<<*(rep.tuple)<<endl;
			}
			else
				cout<<"Requested tuple not found..."<<endl;
			break;
		}
		case Request::Output:{
			cout<<"Output request has been sent"<<endl;
			cout<<"Tuple: "<<endl;
			cout<<*(req->tuple)<<endl;
			linda_output(req);
			break;
		}
		case Request::Read:{
			cout<<"Read request has been sent with timeout: "<< req->timeout <<endl;
			cout<<"Tuple: "<<endl;
			cout<<*(req->tuple)<<endl;
			Reply rep = linda_read(req);
			if(rep.isFound){
				cout<<"Requested tuple popped from tuple space:"<<endl;
				cout<<*(rep.tuple)<<endl;
			}
			else
				cout<<"Requested tuple not found..."<<endl;
			break;
		}
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

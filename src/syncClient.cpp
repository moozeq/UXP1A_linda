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

const std::string commonClientsLogFileName("ClientsCommon.log");

void sig_handler(int signo) {
	if(signo == SIGINT)
		exit(1);
}

struct GuardedCout{

	GuardedCout()
	{
		init();
	}

	std::ostream&
	print_one(std::ostream& os)
	{
	    return os;
	}

	template <class A0, class ...Args>
	std::ostream&
	print_one(std::ostream& os, const A0& a0, const Args& ...args)
	{
	    os << a0;
	    return print_one(os, args...);
	}

	template <class ...Args>
	std::ostream&
	print(std::ostream& os, const Args& ...args)
	{
	    return print_one(os, args...);
	}

	template <class ...Args>
	std::ostream&
	print(const Args& ...args)
	{
		if (sem_wait(coutSemaphore) < 0)
			perror("sem_wait(3) failed on child");
	    std::ostream & os = print(std::cout, "[", getpid(), "]", args...);
	    if (sem_post(coutSemaphore) < 0)
	    			perror("sem_post(3) error on child");
	    return os;
	}

	void init(void)
	{
		coutSemaphore = sem_open(
				semName, O_CREAT | O_RDWR, (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP), 1);
		if (coutSemaphore == SEM_FAILED)
		{
			coutSemaphore = sem_open(semName, O_RDWR);
			if (coutSemaphore == SEM_FAILED) {
				perror("sem_open(3) failed");
				exit(EXIT_FAILURE);
			}
			return;
		}
	}

private:
	sem_t * coutSemaphore;
	const char * semName = "lindaClientSemaphore";
};

int main() {

	signal(SIGINT, sig_handler);

	GuardedCout gCout;
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
				gCout.print("Requested tuple popped from tuple space:\n");
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

	// Another usage
	linda_output({{true, std::string("drugaKrotka")},
		{true, std::string("testowa")},{false, std::string("19")}, {true, std::string("koniec")}});

	// Another usage
	linda_output(Tuple({{true, string("prostaKrotka")}}));

	gCout.print("Client ", getpid(), " exited\n");
	return 0;
}

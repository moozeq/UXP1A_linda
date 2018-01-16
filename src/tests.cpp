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
#include "GuardedCout.h"
#include "tests.h"
#include <vector>

using namespace std;

bool terminateProcess = false;
pid_t parent_pid;
GuardedCout gCout("Test1CppSemaphoreName");

void sig_handler(int sig, siginfo_t *siginfo, void *context) {
	if(sig == SIGINT)
	{
		if(parent_pid != getpid())
		{
			linda_unlink_client_fifo();
			gCout.print("Process ", getpid(), " exiting...\n");
			exit(0);
		}
	}
}

void setSigint(void)
{
	struct sigaction newAction;
	newAction.sa_sigaction = &sig_handler;
	newAction.sa_flags = SA_SIGINFO;
	if (sigaction(SIGINT, &newAction, NULL) < 0) {
		perror ("sigaction");
	}
}

typedef int (*ProcFunc)(void);

int main() {

	struct sigaction newAction;
	newAction.sa_handler = SIG_IGN;
	if (sigaction(SIGINT, &newAction, NULL) < 0) {
		perror ("sigaction");
		return 1;
	}

	parent_pid = getpid();
	pid_t server = -1;
	vector<pid_t> pids;
	std::vector<ProcFunc> test1Functions = {test1_producerF, test1_consumer1F, test1_consumer2F, test1_consumer3F};
	std::vector<ProcFunc> test2Functions = {test2_client1, test2_client2, test2_client3, test2_client4};
	std::vector<ProcFunc> * testFunctions = &test2Functions;

	server = fork();
	if(server == 0)
	{
		int status = execl("./server", "./server", (char *)NULL);
		if(status == -1)
		{
			cerr<<"Failed to run server"<<endl;
			exit(0);
		}
	}
	else if(server < 0)
	{
		cerr<<"Failed to fork to create server"<<endl;
		exit(0);
	}
	else
	{
		sleep(1);	// wait for server to begin running

		for(unsigned i = 0; i < testFunctions->size(); i++) {
		    pids.push_back(fork());
		    if(pids.at(i) < 0) {
		    	gCout.print("Error in fork()\n");
		        exit(1);
		    } else if (pids.at(i) == 0) {
		    	testFunctions->at(i)();
		        exit(0);
		    } else  {
		    	if(parent_pid == getpid() && i == 3)
		    	{
		    		wait(NULL);
		    		gCout.print("Terminating server\n");
		    		//linda_terminate_server();
		    		gCout.print("Server terminate message sent\n");
		    	}
		    }
		}
	}

	if(parent_pid == getpid())
		gCout.print("Proces macierzysty ", getpid(), " konczy...\n");

	gCout.print("Process ", getpid(), " exiting at the end...\n");
	return 0;
}

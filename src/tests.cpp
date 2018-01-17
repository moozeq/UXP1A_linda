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

typedef int (*ProcFunc)(int);

int main(int argc, char * argv[]) {

	int testNumber = 0;
	if(argc > 1)
		testNumber = std::atoi(argv[1]);

	struct sigaction newAction;
	newAction.sa_handler = SIG_IGN;
	if (sigaction(SIGINT, &newAction, NULL) < 0) {
		perror ("sigaction");
		return 1;
	}

	parent_pid = getpid();
	pid_t server = -1;
	vector<pid_t> pids;

	std::vector<std::vector<ProcFunc> *> tests;
	// Vector of pointers to functions for test1, test2, etc...
	std::vector<ProcFunc> test1Functions = {test1_producerF, test1_consumer1F, test1_consumer2F, test1_consumer3F};
	std::vector<ProcFunc> test2Functions = {test2_client0, test2_client1, test2_client2, test2_client3};
	std::vector<ProcFunc> test3Functions = {test3_client0, test3_client1, test3_client2, test3_client3};
	// Vector of pointers to functions for test4
	std::vector<ProcFunc> test4Functions;
	for(unsigned i = 0; i < test4_tuples.size(); ++i)
		test4Functions.push_back(test4_consumer);
	test4Functions.push_back(test4_producer);
	// Vector of pointers to functions for test5
	std::vector<ProcFunc> test5Functions;
	for(unsigned i = 0; i < test5_tuples.size()/2; ++i)
		test5Functions.push_back(test5_patient_consumer);
	for(unsigned i = 0; i < test5_tuples.size()/2; ++i)
		test5Functions.push_back(test5_impatient_consumer);
	test5Functions.push_back(test5_producer);

	// Vector of vector of pointers to functions - for test selection
	tests.push_back(&test1Functions);
	tests.push_back(&test2Functions);
	tests.push_back(&test3Functions);
	tests.push_back(&test4Functions);
	tests.push_back(&test5Functions);
	// Choose test number according to input from user
	std::vector<ProcFunc> * testFunctions;
	if(unsigned(testNumber - 1) < tests.size())
		testFunctions = tests.at(testNumber - 1);	// User give numbers from 1 to n, tests begin from 0
	else
		testFunctions = tests.at(0);

	// Create new process for server
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
		    	testFunctions->at(i)(i);
		        exit(0);
		    } else  {
		    	// If is parent thread and forked all children then wait
		    	if(parent_pid == getpid() && i == (testFunctions->size() - 1))
		    	{
		    		// Wait for all children threads
		    		  for(i = 0; i < pids.size(); ++i)
		    		  {
		    		    wait(NULL);
		    		    cout << "Got " << i+1 << " done" << endl;
		    		  }
		    	}
		    }
		}
	}

	if(parent_pid == getpid())
		gCout.print("Proces macierzysty ", getpid(), " konczy...\n");

	gCout.print("Process ", getpid(), " exiting at the end...\n");
	return 0;
}

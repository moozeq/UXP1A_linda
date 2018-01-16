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

/**
 * 	@brief	Producer process produces tuples with one element which is integer
 * 	in range <0; 100> every second.
 */
int producerF(void)
{
	setSigint();
	std::srand(std::time(nullptr));
	while(!terminateProcess)
	{
		int rand_0_100 = ( std::rand() % ( 101 ) );
		linda_output(Tuple({{false, std::to_string(rand_0_100)}}));
		gCout.print("Producer, tuple's been sent\n");
		sleep(1);
	}
	return 0;
}

/**
 * 	@brief	Consumer1 requests for tuples with one element which is integer <= 50
 * 	(takes half of producer's tuples) every 2 seconds.
 */
int consumer1F(void)
{
	setSigint();
	gCout.print("Consumer1 enter\n");
	while(!terminateProcess)
	{
		Reply r = linda_input(Tuple({{false, std::string("<=50")}}), 10);
		if(r.isFound)
			gCout.print("\tConsumer 1, tuple received: ", r.tuple->elems.at(0).pattern, "\n");
		else
			gCout.print("\tConsumer 1, empty reply...\n");
		sleep(2);
	}
	return 0;
}

/**
 * 	@brief	Consumer2 requests for tuples with one element which is integer > 50
 * 	(takes half of producer's tuples) every 2 seconds.
 */
int consumer2F(void)
{
	setSigint();
	gCout.print("Consumer2 enter\n");
	while(!terminateProcess)
	{
		Reply r = linda_input(Tuple({{false, std::string(">50")}}), 10);
		if(r.isFound)
			gCout.print("\t\tConsumer 2, tuple received: ", r.tuple->elems.at(0).pattern, "\n");
		else
			gCout.print("\t\tConsumer 2, empty reply...\n");
		sleep(2);
	}
	return 0;
}

/**
 * 	@brief	Consumer3 requests for tuples with one element which is integer > 95
 * 	every 2 seconds. It tries to get tuple despite of the fact that consumer1 and
 * 	consumer2 cover all produced tuples
 */
int consumer3F(void)
{
	setSigint();
	gCout.print("Consumer3 enter\n");
	while(!terminateProcess)
	{
		Reply r = linda_input(Tuple({{false, std::string(">95")}}), 10);
		if(r.isFound)
			gCout.print("\t\t\tConsumer 3, tuple received: ", r.tuple->elems.at(0).pattern, "\n");
		else
			gCout.print("\t\t\tConsumer 3, empty reply...\n");
		sleep(2);
	}
	return 0;
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
	pid_t pids[4];
	ProcFunc functions[4] = {producerF, consumer1F, consumer2F, consumer3F};


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

		for(int i = 0; i < 4; i++) {
		    pids[i] = fork();
		    if(pids[i] < 0) {
		    	gCout.print("Error in fork()\n");
		        exit(1);
		    } else if (pids[i] == 0) {
		        functions[i]();
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

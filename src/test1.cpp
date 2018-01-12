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

bool terminateProcess = false;

void sig_handler(int sig, siginfo_t *siginfo, void *context) {
	if(sig == SIGINT)
		terminateProcess = true;
}

int producerF(void)
{
	std::srand(std::time(nullptr));
	while(!terminateProcess)
	{
		int rand_0_100 = ( std::rand() % ( 101 ) );
		linda_output(Tuple({{false, std::to_string(rand_0_100)}}));
		cout<<"Producer, tuple's been sent"<<endl;
		sleep(1);
	}
	exit(0);
}

int consumer1F(void)
{
	cout<<"Consumer1 enter"<<endl;
	while(!terminateProcess)
	{
		Reply r = linda_input(Tuple({{false, std::string("<=50")}}), 10);
		if(r.isFound)
			cout<<"\tConsumer 1, tuple received: "<<r.tuple->elems.at(0).pattern<<endl;
		else
			cout<<"\tConsumer 1, empty reply..."<<endl;
		sleep(2);
	}
	exit(0);
}

int consumer2F(void)
{
	cout<<"Consumer2 enter"<<endl;
	while(!terminateProcess)
	{
		Reply r = linda_input(Tuple({{false, std::string(">50")}}), 10);
		if(r.isFound)
			cout<<"\t\tConsumer 2, tuple received: "<<r.tuple->elems.at(0).pattern<<endl;
		else
			cout<<"\t\tConsumer 2, empty reply..."<<endl;
		sleep(2);
	}
	exit(0);
}

int consumer3F(void)
{
	cout<<"Consumer3 enter"<<endl;
	while(!terminateProcess)
	{
		Reply r = linda_input(Tuple({{false, std::string("13")}}), 10);
		if(r.isFound)
			cout<<"\t\t\tConsumer 3, tuple received: "<<r.tuple->elems.at(0).pattern<<endl;
		else
			cout<<"\t\t\tConsumer 3, empty reply..."<<endl;
		sleep(2);
	}
	exit(0);
}

typedef int (*ProcFunc)(void);

int main() {

	struct sigaction newAction;
	newAction.sa_sigaction = &sig_handler;
	newAction.sa_flags = SA_SIGINFO;
	if (sigaction(SIGINT, &newAction, NULL) < 0) {
			perror ("sigaction");
			return 1;
		}


	pid_t parent_pid = getpid();
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
		        cout<<"Error while fork()"<<endl;
		        exit(1);
		    } else if (pids[i] == 0) {
		        printf("Child (%d): %d\n", i + 1, getpid());
		        functions[i]();
		        exit(0);
		    } else  {
		        wait(NULL);
		    }
		}
	}

	sleep(1);

	if(parent_pid == getpid())
	{
		for(int i = 0; i < 4; ++i)
			waitpid(pids[i], NULL, 0);
		linda_terminate_server();
	}

	if(parent_pid == getpid())
		cout<<"Proces macierzysty"<<endl;
//	// Another usage
//	linda_output({{true, std::string("drugaKrotka")},
//		{true, std::string("testowa")},{false, std::string("19")}, {true, std::string("koniec")}});
//
//	// Another usage
//	linda_output(Tuple({{true, string("prostaKrotka")}}));

	cout<<"Process" << getpid() << " exited" << endl;
	return 0;
}

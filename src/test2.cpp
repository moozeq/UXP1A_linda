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

using namespace std;

extern bool terminateProcess;
extern GuardedCout gCout;
extern void setSigint(void);

std::vector<Tuple> test2_tuples = {Tuple({string("pierwszaKrotka")}),
		Tuple({17}), Tuple({string("drugaKrotka"), 99}), Tuple({4, 677, string("krotkaKoncowa")})};

int test2_client1(void)
{
	setSigint();
	int tupleNumber = 0;
	while(!terminateProcess)
	{
		linda_output(test2_tuples.at(tupleNumber));
		gCout.print("Producer, tuple's been sent\n");
		sleep(1);
		Reply r = linda_input(test2_tuples.at(tupleNumber + 1), 5);
		if(!r.isFound)
			gCout.print("Client1 - tuple timeout exceeded!\n");
		else
			gCout.print("Client1 - received requested tuple\n");
	}
	return 0;
}

int test2_client2(void)
{
	setSigint();
	int tupleNumber = 1;
	while(!terminateProcess)
	{
		linda_output(test2_tuples.at(tupleNumber));
		gCout.print("Producer, tuple's been sent\n");
		sleep(1);
		Reply r = linda_input(test2_tuples.at(tupleNumber + 1), 5);
		if(!r.isFound)
			gCout.print("Client1 - tuple timeout exceeded!\n");
		else
			gCout.print("Client1 - received requested tuple\n");
	}
	return 0;
}

int test2_client3(void)
{
	setSigint();
	int tupleNumber = 2;
	while(!terminateProcess)
	{
		linda_output(test2_tuples.at(tupleNumber));
		gCout.print("Producer, tuple's been sent\n");
		sleep(1);
		Reply r = linda_input(test2_tuples.at(tupleNumber + 1), 5);
		if(!r.isFound)
			gCout.print("Client1 - tuple timeout exceeded!\n");
		else
			gCout.print("Client1 - received requested tuple\n");
	}
	return 0;
}

int test2_client4(void)
{
	setSigint();
	int tupleNumber = 3;
	while(!terminateProcess)
	{
		linda_output(test2_tuples.at(tupleNumber));
		gCout.print("Producer, tuple's been sent\n");
		sleep(1);
		Reply r = linda_input(test2_tuples.at(0), 5);
		if(!r.isFound)
			gCout.print("Client1 - tuple timeout exceeded!\n");
		else
			gCout.print("Client1 - received requested tuple\n");
	}
	return 0;
}

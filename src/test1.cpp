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

/**
 * 	@brief	Producer process produces tuples with one element which is integer
 * 	in range <0; 100> every second.
 */
int test1_producerF(void)
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
int test1_consumer1F(void)
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
int test1_consumer2F(void)
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
int test1_consumer3F(void)
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

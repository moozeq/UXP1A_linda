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

std::vector<Tuple> test5_tuples = {Tuple({10}), Tuple({11}), Tuple({12}), Tuple({13}), Tuple({14}), Tuple({15}), Tuple({16}), Tuple({17}), Tuple({18}), Tuple({19})};

int test5_producer(int)
{
	setSigint();
	int tupleNumber = 0;
	while(!terminateProcess)
	{
		linda_output(test5_tuples.at(tupleNumber));
		gCout.print("Producer - tuple no", tupleNumber, " has been sent\n");
		tupleNumber = (tupleNumber + 1) % 10;
		sleep(1);
	}
	return 0;
}


int test5_patient_consumer(int tupleNumber)
{
	setSigint();
	while(!terminateProcess)
	{
		Reply r = linda_input(test5_tuples.at(tupleNumber), test5_tuples.size() + 1);		// timeout is greater than producer cycle
		if(r.isFound)
			gCout.print("Consumer ", tupleNumber, " - tuple no", tupleNumber, " received\n");
		else
			gCout.print("\t\tConsumer ", tupleNumber, " - tuple no", tupleNumber, " not found...\n");
	}
	return 0;
}

int test5_impatient_consumer(int tupleNumber)
{
	setSigint();
	while(!terminateProcess)
	{
		Reply r = linda_input(test5_tuples.at(tupleNumber), test5_tuples.size()/2 + 1);		// timeout is less than producer cycle
		if(r.isFound)
			gCout.print("Consumer ", tupleNumber, " - tuple no", tupleNumber, " received\n");
		else
			gCout.print("\t\tConsumer ", tupleNumber, " - tuple no", tupleNumber, " not found...\n");
	}
	return 0;
}

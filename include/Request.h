/*
 * Request.h
 *
 *  Created on: Dec 15, 2017
 *      Author: piotr
 */

#ifndef REQUEST_H_
#define REQUEST_H_

#include <unistd.h>
#include <time.h>

#include "Tuple.h"

enum RequestType{
	inputTuple,
	outputTuple,
	readTuple
};

class Request
{
public:
	Request();
	virtual ~Request();

private:
	pid_t procId;
	RequestType reqType;
	time_t timeout;
	Tuple tuple;
};

#endif /* REQUEST_H_ */

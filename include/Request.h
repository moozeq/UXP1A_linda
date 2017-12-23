#ifndef REQUEST_H_
#define REQUEST_H_

#include "Tuple.h"

/**
 * 	@brief	Class that represents client request. Request object
 * 	agregates tuple which is the subject of this request.
 */
class Request
{
public:
	unsigned procId;
	unsigned reqType; //0 - input, 1 - output, 2 - read
	unsigned timeout; //in seconds
	Tuple* tuple;

	friend std::ostream& operator<<(std::ostream &os, const Request& req);
	friend std::istream& operator>>(std::istream &is, Request& req);
	Request() {
		procId = 0;
		reqType = 0;
		timeout = 0;
		tuple = new Tuple();
	}
	~Request(){
		if(tuple != nullptr)
			delete tuple;
	}
};

#endif /* REQUEST1_H_ */

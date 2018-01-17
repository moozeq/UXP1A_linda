#ifndef REPLY_H_
#define REPLY_H_

#include "Tuple.h"

/**
 * 	@brief	Class that represents server response for client's request.
 * 	Reply object contains tuple which is passed back to client and information
 * 	whether the requested tuple was found or not.
 */
class Reply
{
public:
	friend std::ostream& operator<<(std::ostream &os, const Reply& rep);
	friend std::istream& operator>>(std::istream &is, Reply& rep);
	Reply() {
		isFound = false;
		tuple = new Tuple();
	}
	Reply(const Reply & other){
		this->isFound = other.isFound;
		this->tuple = new Tuple(*(other.tuple));
	}
	~Reply(){
		if(tuple != nullptr)
			delete tuple;
	}
	Reply & operator=(const Reply & other);
	void setTuple(const Tuple * _tuple);

	bool isFound;
	const Tuple* tuple;
};
#endif /* REPLY_H_ */

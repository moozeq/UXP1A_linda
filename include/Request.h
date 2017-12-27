#ifndef REQUEST_H_
#define REQUEST_H_

#include "Tuple.h"
#include <type_traits>

/**
 * 	@brief	operator>> for conversion from int to any enum type.
 */
template<typename T>
typename std::enable_if<std::is_enum<T>::value, std::istream&>::type
operator >>(std::istream &is, T& enumVariable)
{
  int intValue;
  is >> intValue;
  enumVariable = static_cast<T>(intValue); // note the explicit cast to make it legal
  return is;
}

/**
 * 	@brief	Class that represents client request. Request object
 * 	agregates tuple which is the subject of this request.
 */
class Request
{
public:
	enum ReqType{
		Input,
		Output,
		Read
	};
	unsigned procId;
	ReqType reqType; //0 - input, 1 - output, 2 - read
	unsigned timeout; //in seconds
	const Tuple* tuple;

	friend std::ostream& operator<<(std::ostream &os, const Request& req);
	friend std::istream& operator>>(std::istream &is, Request& req);
	Request() {
		procId = 0;
		reqType = Input;
		timeout = 0;
		tuple = new Tuple();
	}
	~Request(){
		if(tuple != nullptr)
			delete tuple;
	}

	void setTuple(Tuple * _tuple);
};


std::ostream& operator<<(std::ostream &os, const Request& req);
std::istream& operator>>(std::istream &is, Request& req);

#endif /* REQUEST1_H_ */

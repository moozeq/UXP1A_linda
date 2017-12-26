#ifndef TUPLE_H_
#define TUPLE_H_

#include <unistd.h>
#include <time.h>
#include <string>
#include <vector>
#include <fstream>

class Elem;

/**
 * 	@brief	Class that represents single tuple in tuple space.
 * 	Contains vector of Elem objects.
 */
class Tuple
{
public:
	std::vector<Elem> elems;

	bool operator==(const Tuple & other);
};

/**
 * 	@brief	Class that represents single element in the tuple.
 * 	It is defined as a string or integer data type using its members.
 */
class Elem
{
public:
	bool isString;
	std::string pattern;

	Elem(bool isString, std::string pattern) {
		this->isString = isString;
		this->pattern = pattern;
	}
	Elem(bool isString, char* pattern) {
		this->isString = isString;
		this->pattern = pattern;
	}
};

#endif /* TUPLE_H_ */

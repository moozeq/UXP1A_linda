#ifndef INCLUDE_ELEM_H_
#define INCLUDE_ELEM_H_

#include <string>

/**
 * 	@brief	Class that represents single element in the tuple.
 * 	It is defined as a string or integer data type using its members.
 */
class Elem
{
public:
	bool isString;
	std::string pattern;

	Elem(bool _isString, std::string _pattern) :
		isString(_isString), pattern(_pattern)	{}
	Elem(bool _isString, char* _pattern) :
		isString(_isString), pattern(_pattern)	{}
	Elem(int _pattern) :
			isString(false), pattern(std::to_string(_pattern))	{}
	Elem(std::string _pattern) :
			isString(true), pattern(_pattern)	{}
};

#endif /* INCLUDE_ELEM_H_ */

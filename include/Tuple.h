#ifndef TUPLE_H_
#define TUPLE_H_

#include <unistd.h>
#include <time.h>
#include <ctime>
#include <string>
#include <vector>
#include <fstream>
#include <initializer_list>
#include <functional>
#include <ostream>
#include "Elem.h"

/**
 * 	@brief	Class that represents single tuple in tuple space.
 * 	Contains vector of Elem objects.
 */
class Tuple
{
public:
	std::vector<Elem> elems;

	friend std::ostream & operator<<(std::ostream & os, const Tuple & tuple);

	Tuple(){}
	Tuple(const Tuple & t);
	Tuple(std::initializer_list<Elem> init) : elems(init)	{}
	Tuple & operator=(const Tuple & t);
	const size_t getHash() const;
};

/**
 * 	@brief	Function which provides operator== for Tuple objects comparison.
 */
bool operator ==(const Tuple & first, const Tuple & second);

bool operator!=(const Tuple & first, const Tuple & second);

/**
 * 	@brief	operator<< for Tuple class.
 */
std::ostream & operator<<(std::ostream & os, const Tuple & tuple);

#endif /* TUPLE_H_ */

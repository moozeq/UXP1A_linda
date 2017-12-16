/*
 * Tuple.h
 *
 *  Created on: Dec 15, 2017
 *      Author: piotr
 */

#ifndef TUPLE_H_
#define TUPLE_H_

#include <string>
#include <vector>

struct Tuple
{
public:
	Tuple();
	virtual ~Tuple();

	struct Elem
	{
		/* type: 0 - integer, 1 - string */
		bool type;
		std::string pattern;
	};

	std::vector<Elem> elements;
	size_t elementsCount;
};

#endif /* TUPLE_H_ */

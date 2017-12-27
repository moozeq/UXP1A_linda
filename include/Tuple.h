#ifndef TUPLE_H_
#define TUPLE_H_

#include <unistd.h>
#include <time.h>
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
	Tuple(std::initializer_list<Elem> init)
	      : elems(init)
	    { }
	~Tuple(){}
};

bool operator ==(const Tuple & first, const Tuple & second);

std::ostream & operator<<(std::ostream & os, const Tuple & tuple);

namespace std
{
    template <>
    struct hash<Tuple>
    {
        size_t operator()(const Tuple& t) const
        {
            size_t hashValue = t.elems.size();
            for(const Elem & e : t.elems)
            {
            	hashValue ^= (hash<std::string>()(e.pattern) << (int)e.isString);
            }
            return hashValue;
        }
    };
}

#endif /* TUPLE_H_ */

#ifndef INCLUDE_TUPLESPACE_H_
#define INCLUDE_TUPLESPACE_H_

#include <unordered_set>
#include <functional>
#include "Tuple.h"

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

class TupleSpace
{
public:
	std::pair<std::unordered_set<Tuple>::iterator, bool> insert(const Tuple & tuple);

private:
	std::unordered_set<Tuple> tuples;
};

#endif /* INCLUDE_TUPLESPACE_H_ */

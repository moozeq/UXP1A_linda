#include "Tuple.h"

bool operator ==(const Tuple & first, const Tuple & second){
	if(first.elems.size() != second.elems.size())
		return false;
	auto it1 = first.elems.begin();
	auto it2 = second.elems.begin();
	for(; it1 != first.elems.end() && it2 != second.elems.end(); ++it1, ++it2){
		if(it1->isString != it2->isString ||
				it1->pattern != it2->pattern)
			return false;
	}
	return true;
}

std::ostream & operator<<(std::ostream & os, const Tuple & tuple)
{
	for(const Elem & e : tuple.elems)
		os << std::boolalpha << e.isString << ", " << e.pattern << std::endl;
	return os;
}

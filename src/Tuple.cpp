#include "Tuple.h"

bool Tuple::operator==(const Tuple & other){
	if(elems.size() != other.elems.size())
		return false;
	auto it1 = elems.begin();
	auto it2 = other.elems.begin();
	for(; it1 != elems.end() && it2 != other.elems.end(); ++it1, ++it2){
		if(it1->isString != it2->isString ||
				it1->pattern != it2->pattern)
			return false;
	}
	return true;
}

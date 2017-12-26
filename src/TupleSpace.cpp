#include "TupleSpace.h"

std::pair<std::unordered_set<Tuple>::iterator, bool> TupleSpace::insert(const Tuple & tuple)
{
	//this->tuples.insert(tuple);
	return std::pair<std::unordered_set<Tuple>::iterator, bool>(tuples.begin(), true);
}

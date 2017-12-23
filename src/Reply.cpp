#include "Reply.h"

/**
 * 	@brief 	Gets data from the reply and pushes it into ostream.
 *
 * 	@param	os is ostream object where data is pushed to
 * 	@param	rep is Reply object which provides reply data
 */
std::ostream& operator<<(std::ostream &os, const Reply& rep) {
	os << rep.isFound << std::endl;
	os << rep.tuple->elems.size() << std::endl;
	for (unsigned i = 0; i < rep.tuple->elems.size(); ++i) {
		os << rep.tuple->elems[i].isString << std::endl;
		os << rep.tuple->elems[i].pattern << std::endl;
	}
	return os;
}

/**
 *	@brief	Gets request from istream and pushes data into given Reply object.
 *
 *	@param is is ifstream object wich contains Reply data
 *	@param rep is empty Reply object to store reply data
 */
std::istream& operator>>(std::istream &is, Reply& rep) {
	is >> rep.isFound;

	Tuple *tuple = new Tuple();
	unsigned elemsCount;
	is >> elemsCount;
	for (unsigned i = 0; i < elemsCount; ++i) {
		bool isString;
		std::string tmp;
		is >> isString;
		is >> tmp;
		Elem el = Elem(isString, tmp);
		tuple->elems.push_back(el);
	}
	if(rep.tuple != nullptr)
		delete rep.tuple;
	rep.tuple = tuple;
	return is;
}

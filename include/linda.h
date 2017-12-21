#include <unistd.h>
#include <time.h>
#include <string>
#include <vector>
#include <fstream>

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

class Tuple
{
public:
	std::vector<Elem> elems;
};

class Request
{
public:
	unsigned procId;
	unsigned reqType; //0 - input, 1 - output, 2 - read
	unsigned timeout; //in seconds
	Tuple* tuple;

	friend std::ostream& operator<<(std::ostream &os, const Request& req);
	friend std::istream& operator>>(std::istream &is, Request& req);
	Request() {
		procId = 0;
		reqType = 0;
		timeout = 0;
		tuple = new Tuple();
	}
};

std::ostream& operator<<(std::ostream &os, const Request& req) {
	os << req.procId << std::endl;
	os << req.reqType << std::endl;
	os << req.timeout << std::endl;
	os << req.tuple->elems.size() << std::endl;
	for (unsigned i = 0; i < req.tuple->elems.size(); ++i) {
		os << req.tuple->elems[i].isString << std::endl;
		os << req.tuple->elems[i].pattern << std::endl;
	}
	return os;
}

std::istream& operator>>(std::istream &is, Request& req) {
	is >> req.procId;
	is >> req.reqType;
	is >> req.timeout;

	Tuple *tuple = new Tuple();
	unsigned elemsCount;
	is >> elemsCount;
	for (unsigned i = 0; i < elemsCount; ++i) {
		bool isString;
		std::string tmp;
		is >> isString;
		is >> tmp;
		Elem *el = new Elem(isString, tmp);
		tuple->elems.push_back(*el);
	}
	req.tuple = tuple;
	return is;
}

class Reply
{
public:
	bool isFound;
	Tuple* tuple;
	friend std::ostream& operator<<(std::ostream &os, const Reply& rep);
	friend std::istream& operator>>(std::istream &is, Reply& rep);
	Reply() {
		isFound = false;
		tuple = new Tuple();
	}
};

std::ostream& operator<<(std::ostream &os, const Reply& rep) {
	os << rep.isFound << std::endl;
	os << rep.tuple->elems.size() << std::endl;
	for (unsigned i = 0; i < rep.tuple->elems.size(); ++i) {
		os << rep.tuple->elems[i].isString << std::endl;
		os << rep.tuple->elems[i].pattern << std::endl;
	}
	return os;
}

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
		Elem *el = new Elem(isString, tmp);
		tuple->elems.push_back(*el);
	}
	rep.tuple = tuple;
	return is;
}

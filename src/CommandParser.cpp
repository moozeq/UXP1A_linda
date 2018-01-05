/*
 * CommandParser.cpp
 *
 *  Created on: Jan 4, 2018
 *      Author: ted
 */

#include "CommandParser.h"
#include <iostream>
#include <string>
#include <sstream>
#include "Request.h"
#include "Tuple.h"
#include "Elem.h"

using namespace std;

void CommandParser::showOptions() {
	cout
		 << "\tinput(tuple pattern, timeout) to get and remove tuple from tuples space" << endl
		 << "\toutput(tuple) to insert tuple into tuples space" << endl
		 << "\tread(tuple pattern, timeout) to get tuple from tuples space without removing it" << endl << endl
		 << "Where (tuple pattern) is a demanded tuple consists strings/integers patterns as follows: " << endl
		 << "\tstring:\"<string pattern>\", integer:<integer pattern>" << endl
		 << "> ";
}

bool CommandParser::parseCommand(string line, Request* req) {
	stringstream ss(line);
	req->procId = getpid();
	req->timeout = 0;

	string command, type, pattern, tupleStr;
	while (ss.peek() == ' ')
		ss.ignore();
	getline(ss, command, '(');
	while (command.back() == ' ')
		command.pop_back();
	if (command == "input")
		req->reqType = Request::Input;
	else if (command == "output")
		req->reqType = Request::Output;
	else if (command == "read")
		req->reqType = Request::Read;
	else
		return false;

	Tuple* tup = new Tuple();
	while (ss.peek() == ' ')
		ss.ignore();
	getline(ss, tupleStr);
	stringstream st(tupleStr);
	while (getline(st, type, ':')) {
		while (type.back() == ' ')
			type.pop_back();
		if (type != "string" && type != "integer") {
			if (req->reqType == Request::Input || req->reqType == Request::Read) { //timeout
				getline(st, type, ')');
				while (type.back() == ' ' || type.back() == ')')
					type.pop_back();
				req->timeout = stoul(pattern);
				req->setTuple(tup);
				return true;
			}
			cout << "Wrong input" << endl;
			delete tup;
			return false;
		}
		if (type == "string") {
			int quotMarks = 0;
			while (st.peek() == ' ')
				st.ignore();
			if(st.peek() == '"') {
				st.ignore();
				++quotMarks;
			}
			getline(st, pattern, ',');
			while (pattern.back() == ' ')
				pattern.pop_back();
			if (pattern.back() == ')') {
				cout << "Wrong input, check out brackets (<tuple pattern>)" << endl;
				delete tup;
				return false;
			}
			if(pattern.back() == '"') {
				pattern.pop_back();
				++quotMarks;
			}
			if(quotMarks != 2) {
				cout << "Wrong input, check out quot marks (string:\"<...>\")" << endl;
				delete tup;
				return false;
			}
			Elem* el = new Elem(true, pattern);
			tup->elems.push_back(*el);
		}
		else if (type == "integer") {
			while (st.peek() == ' ')
				st.ignore();
			getline(st, pattern, ',');
			while (pattern.back() == ' ')
				pattern.pop_back();
			if (pattern.back() == ')') {
				cout << "Wrong input" << endl;
				delete tup;
				return false;
			}
			Elem* el = new Elem(false, pattern);
			tup->elems.push_back(*el);
		}
		else {
			cout << "Wrong input" << endl;
			delete tup;
			return false;
		}
		while (st.peek() == ' ')
			st.ignore();
	}
	req->setTuple(tup);
	return true;
}

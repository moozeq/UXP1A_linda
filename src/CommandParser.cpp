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

bool CommandParser::checkIfExit(string line) {
	string ex = "exit";
	return line == ex;
}

bool CommandParser::parseCommand(string line, Request* req) {
	req->procId = getpid();
	req->timeout = 0;

	stringstream ss(line);
	string command, type, pattern, tupleStr; //command(tupleStr) | tupleStr = type:pattern...

	while (ss.peek() == ' ') //ignore whitespace before command
		ss.ignore();
	getline(ss, command, '(');
	while (command.back() == ' ') //ignore whitespace after command
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
	while (ss.peek() == ' ') //ignore whitespace before tupleStr
		ss.ignore();
	getline(ss, tupleStr); //get whole line after command as tupleStr
	stringstream st(tupleStr);
	while (getline(st, type, ':')) { //get type (str/int) and remove whitespace
		while (type.back() == ' ')
			type.pop_back();
		if (type == "string") {
			int quotMarks = 0; //check if right syntax -> (string:"...")
			while (st.peek() == ' ')
				st.ignore();
			if(st.peek() == '"') {
				st.ignore();
				++quotMarks;
			}
			getline(st, pattern, ','); //get pattern with 2nd quot mark
			while (pattern.back() == ' ')
				pattern.pop_back();
			if (pattern.back() == ')') { //end of tupleStr where should be timeout or end of output
				if (req->reqType != Request::Output) { //not output, wrong input
					cout << "Wrong input, in the end should be timeout" << endl;
					delete tup;
					return false;
				}
				else
					pattern.pop_back(); //pop ')' from output
			}

			if(pattern.back() == '"') { //remove quot mark
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
			while (st.peek() == ' ') //remove whitespace
				st.ignore();
			getline(st, pattern, ',');
			while (pattern.back() == ' ')
				pattern.pop_back();
			if (pattern.back() == ')') { //end of tupleStr where should be timeout or end of output
				if (req->reqType != Request::Output) { //not output, wrong input
					cout << "Wrong input, in the end should be timeout" << endl;
					delete tup;
					return false;
				}
				else
					pattern.pop_back(); //pop ')' from output
			}
			Elem* el = new Elem(false, pattern);
			tup->elems.push_back(*el);
		}
		else { //didn't recognize type, not string nor integer, may be timeout
			if ((req->reqType == Request::Input || req->reqType == Request::Read) && tup->elems.size() > 0) { //timeout in read & input
				getline(st, type, ')');
				while (type.back() == ' ' || type.back() == ')') //remove bracket and whitespace from timeout var
					type.pop_back();
				try {
				req->timeout = stoul(pattern); //convert string to unsigned
				}
				catch (invalid_argument& e) {
					cout << "Wrong input" << endl;
					delete tup;
					return false;
				}
				req->setTuple(tup);
				return true; //timeout it's last value in tupleStr
			}
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

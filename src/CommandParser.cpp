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
		 << "Available commands: " << endl
		 << "\tinput(tuple pattern, timeout) to get and remove tuple from tuples space" << endl
		 << "\toutput(tuple) to insert tuple into tuples space" << endl
		 << "\tread(tuple pattern, timeout) to get tuple from tuples space without removing it" << endl << endl
		 << "Where (tuple pattern) is a demanded tuple consists strings/integers patterns as follows: " << endl
		 << "\tstring:\"<string pattern>\", integer:<integer pattern>, string:*, integer:*" << endl
		 << "> ";
}

bool CommandParser::checkIfExit(string line) {
	string ex = "exit";
	return line == ex;
}

void CommandParser::changeToDefSendRcvMessage(std::string & command)
{
	if(command == "defs")
		command = "output(integer:13,string:\"siema\")";
	else if(command == "defr")
		command = "input(integer:13,string:\"siema\",100)";
}


void skipWhsp(string& str) {
	while (str.back() == ' ')
		str.pop_back();
}

void skipWhsp(stringstream& ss) {
	while (ss.peek() == ' ')
		ss.ignore();
}

bool CommandParser::parseCommand(string line, Request* req) {
	req->procId = getpid();
	req->timeout = 0;

	stringstream ss(line);
	string command, type, pattern, tupleStr; //command(tupleStr) | tupleStr = type:pattern...

	skipWhsp(ss);
	getline(ss, command, '(');
	skipWhsp(command);

	if (command == "input")
		req->reqType = Request::Input;
	else if (command == "output")
		req->reqType = Request::Output;
	else if (command == "read")
		req->reqType = Request::Read;
	else
		return false;

	Tuple* tup = new Tuple();
	skipWhsp(ss);
	getline(ss, tupleStr); //get whole line after command as tupleStr
	stringstream st(tupleStr);
	while (getline(st, type, ':')) { //get type (str/int) and remove whitespace
		skipWhsp(type);
		if (type == "string") {
			int quotMarks = 0; //check if right syntax -> (string:"...")
			skipWhsp(st);
			if(st.peek() == '"') {
				st.ignore();
				++quotMarks;
			}
			getline(st, pattern, ','); //get pattern with 2nd quot mark
			skipWhsp(pattern);
			if (pattern.back() == ')') { //end of tupleStr, timeout or without
				pattern.pop_back(); //pop ')'
				skipWhsp(pattern);
			}
			if(pattern.back() == '"') { //remove quot mark
				pattern.pop_back();
				++quotMarks;
			}
			if(quotMarks != 2 && !(pattern.size() == 1 && pattern.back() == '*')) { //can be only: ".." or * as pattern
				cout << "Wrong input, check out quot marks (string:\"<...>\")" << endl;
				delete tup;
				return false;
			}
			Elem el(true, pattern);
			tup->elems.push_back(el);
		}
		else if (type == "integer") {
			skipWhsp(st);
			getline(st, pattern, ',');
			skipWhsp(pattern);
			if (pattern.back() == ')') {//end of tupleStr, timeout or without
				pattern.pop_back(); //pop ')'
				skipWhsp(pattern);
			}
			Elem el(false, pattern);
			tup->elems.push_back(el);
		}
		else { //didn't recognize type, not string nor integer, may be timeout
			if ((req->reqType == Request::Input || req->reqType == Request::Read) && tup->elems.size() > 0) { //timeout in read & input
				getline(st, type, ')');
				while (type.back() == ' ' || type.back() == ')') //remove bracket and whitespace from timeout var
					type.pop_back();
				skipWhsp(type);
				if (type.back() == ')')
					type.pop_back();
				skipWhsp(type);
				try {
					req->timeout = stoul(type); //convert string to unsigned
				}
				catch (invalid_argument& e) {
					cout << "Wrong input, wrong timeout" << endl;
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
		skipWhsp(st);
	}
	req->setTuple(tup);
	return true;
}

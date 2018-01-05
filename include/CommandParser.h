/*
 * CommandParser.h
 *
 *  Created on: Jan 4, 2018
 *      Author: ted
 */

#ifndef COMMANDPARSER_H_
#define COMMANDPARSER_H_
#include <string>
#include "Request.h"

class CommandParser {
public:
	static bool parseCommand(std::string line, Request* req);
	static void showOptions();

};

#endif /* COMMANDPARSER_H_ */

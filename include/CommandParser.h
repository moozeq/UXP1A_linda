#ifndef COMMANDPARSER_H_
#define COMMANDPARSER_H_
#include <string>
#include "Request.h"

/**
 * 	@brief	Class which contains static methods to parse
 * 	commands from user's terminal and create demanded Request
 */
class CommandParser {
public:
	static bool parseCommand(std::string line, Request* req);
	static bool checkIfExit(std::string line);
	static void showOptions();
};

#endif /* COMMANDPARSER_H_ */

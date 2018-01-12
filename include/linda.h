#include "CommandParser.h"
#include "Elem.h"
#include "Reply.h"
#include "Request.h"
#include "Tuple.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <signal.h>
#include <semaphore.h>
#include <ctime>

/**
 * @brief	Pop tuple by passing Request object pointer.
 *
 * @param	req is pointer to Request of Request::Input type
 *
 * Sample usage:
 *
 * Request * r = new Request(new Tuple(), Request::Input, 10);
 * linda_input(req);
 * delete req;
 */
void linda_input(const Request * req);

/**
 * @brief	Pop tuple given as argument.
 *
 * @param	tuple is Tuple to be popped from tuple space
 * @param 	timeout is timout for waiting for the reply
 *
 * Sample usage:
 *
 * linda_input(Tuple(), 10);
 */
void linda_input(Tuple tuple, std::time_t timeout);

/**
 * @brief	Pop tuple given as Elem's initializer list.
 *
 * @param	init is Initializer list of the Elem objects of the tuple to be popped
 * @param 	timeout is timout for waiting for the reply
 *
 * Sample usage:
 *
 * linda_input({{true, std::string("krotka")},
		{true, std::string("testowa")},{false, std::string("3")}}, 10);
 */
void linda_input(const std::initializer_list<Elem> & init, std::time_t timeout);

/**
 * @brief	Send tuple by passing Request object pointer.
 *
 * @param	req is pointer to Request of Request::Output type
 *
 *  Sample usage:
 *
 * Request * r = new Request(new Tuple(), Request::Output);
 * linda_output(req);
 * delete req;
 */
void linda_output(const Request * req);

/**
 * @brief	Send tuple given as argument.
 *
 * @param	tuple is Tuple to be sent to tuple space
 *
 * Sample usage:
 *
 * linda_output(Tuple());
 */
void linda_output(Tuple tuple);

/**
 * @brief	Send tuple given as Elem's initializer list.
 *
 * @param	init is Initializer list of the Elem objects of the tuple to be sent
 *
 * Sample usage:
 *
 * linda_output({{true, std::string("krotka")},
		{true, std::string("testowa")},{false, std::string("3")}});
 */
void linda_output(const std::initializer_list<Elem> & init);

/**
 * @brief	Read tuple by passing Request object pointer.
 *
 * @param	req is pointer to Request of Request::Read type
 *
 * Sample usage:
 *
 * Request * r = new Request(new Tuple(), Request::Read, 10);
 * linda_read(req);
 * delete req;
 */
void linda_read(Request * req);

/**
 * @brief	Read tuple given as argument.
 *
 * @param	tuple is Tuple to be read from tuple space
 * @param 	timeout is timout for waiting for the reply
 *
 * Sample usage:
 *
 * linda_read(Tuple(), 10);
 */
void linda_read(Tuple tuple, std::time_t timeout);

/**
 * @brief	Read tuple given as Elem's initializer list.
 *
 * @param	init is Initializer list of the Elem objects of the tuple to be read
 * @param 	timeout is timout for waiting for the reply
 *
 * Sample usage:
 *
 * linda_read({{true, std::string("krotka")},
		{true, std::string("testowa")},{false, std::string("3")}}, 10);
 */
void linda_read(const std::initializer_list<Elem> & init, std::time_t timeout);


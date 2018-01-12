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

void linda_input(Tuple tuple, std::time_t timeout);
void linda_output(Tuple tuple);
void linda_read(Tuple tuple, std::time_t timeout);

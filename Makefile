# Makefile
# 	Author: piotr

# Compiler
CC     = g++

# Include directory
INC = -I./include

# Source files directory
SRC_DIR = src

# Compiler flags
CFLAGS = -Wall $(INC) -std=c++11 -pthread

#
# Project files
#
COMMONSRCS = $(SRC_DIR)/Tuple.cpp $(SRC_DIR)/Request.cpp $(SRC_DIR)/Reply.cpp $(SRC_DIR)/CommandParser.cpp
CLIENTSRCS = $(COMMONSRCS) $(SRC_DIR)/client.cpp $(SRC_DIR)/linda.cpp
SERVERSRCS = $(COMMONSRCS) $(SRC_DIR)/server.cpp $(SRC_DIR)/GuardedQueue.cpp $(SRC_DIR)/GuardedList.cpp
TESTSSRCS = $(COMMONSRCS) $(SRC_DIR)/test1.cpp $(SRC_DIR)/linda.cpp

#
# Object files
#
CLIENTOBJS = $(CLIENTSRCS:.cpp=.o)
SERVEROBJS = $(SERVERSRCS:.cpp=.o)
TESTSOBJS = $(TESTSSRCS:.cpp=.o)
CLIENTEXE  = client
SERVEREXE = server
TESTSEXE = test1

#
# Debug build settings
#
DBGDIR = debug
DBGEXECLIENT = $(DBGDIR)/$(CLIENTEXE)
DBGEXESERVER = $(DBGDIR)/$(SERVEREXE)
DBGEXETESTS = $(DBGDIR)/$(TESTSEXE)
DBGOBJSCLIENT = $(addprefix $(DBGDIR)/, $(CLIENTOBJS))
DBGOBJSSERVER = $(addprefix $(DBGDIR)/, $(SERVEROBJS))
DBGOBJSTESTS = $(addprefix $(DBGDIR)/, $(TESTSOBJS))
DBGCFLAGS = -g -O0 -DDEBUG

#
# Release build settings
#
RELDIR = release
RELEXECLIENT = $(RELDIR)/$(CLIENTEXE)
RELEXESERVER = $(RELDIR)/$(SERVEREXE)
RELEXETESTS = $(RELDIR)/$(TESTSEXE)
RELOBJSCLIENT = $(addprefix $(RELDIR)/, $(CLIENTOBJS))
RELOBJSSERVER = $(addprefix $(RELDIR)/, $(SERVEROBJS))
RELOBJSTESTS = $(addprefix $(RELDIR)/, $(TESTSOBJS))
RELCFLAGS = -O3 -DNDEBUG

.PHONY: all clean debug prep release remake

# Default build
all: prep release

#
# Debug rules
#
debug: $(DBGEXECLIENT) $(DBGEXESERVER) $(DBGEXETESTS)

$(DBGEXECLIENT): $(DBGOBJSCLIENT)
	$(CC) $(CFLAGS) $(DBGCFLAGS) -o $(DBGEXECLIENT) $^

$(DBGEXESERVER): $(DBGOBJSSERVER)
	$(CC) $(CFLAGS) $(DBGCFLAGS) -o $(DBGEXESERVER) $^
	
$(DBGEXETESTS): $(DBGOBJSTESTS)
	$(CC) $(CFLAGS) $(DBGCFLAGS) -o $(DBGEXETESTS) $^

$(DBGDIR)/%.o: %.cpp
	$(CC) -c $(CFLAGS) $(DBGCFLAGS) -o $@ $<

#
# Release rules
#
release: $(RELEXECLIENT) $(RELEXESERVER) $(RELEXETESTS)

$(RELEXECLIENT): $(RELOBJSCLIENT)
	$(CC) $(CFLAGS) $(RELCFLAGS) -o $(RELEXECLIENT) $^

$(RELEXESERVER): $(RELOBJSSERVER)
	$(CC) $(CFLAGS) $(RELCFLAGS) -o $(RELEXESERVER) $^

$(RELEXETESTS): $(RELOBJSTESTS)
	$(CC) $(CFLAGS) $(RELCFLAGS) -o $(RELEXETESTS) $^

$(RELDIR)/%.o: %.cpp
	$(CC) -c $(CFLAGS) $(RELCFLAGS) -o $@ $<

#
# Other rules
#
prep:
	@mkdir -p $(DBGDIR) $(RELDIR) $(DBGDIR)/src $(RELDIR)/src

remake: clean all

clean:
	rm -f $(RELOBJSCLIENT) $(RELOBJSSERVER) $(RELEXECLIENT) $(RELEXESERVER) $(DBGOBJSCLIENT) $(DBGOBJSSERVER) $(DBGEXECLIENT) $(DBGEXESERVER) $(RELOBJSTESTS) $(RELEXETESTS) $(DBGOBJSTESTS) $(DBGEXETESTS)
	
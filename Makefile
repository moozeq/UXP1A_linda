# Makefile
# 	Author: piotr

# Compiler
CC     = g++

# Include directory
INC = -I./include

# Source files directory
SRC_DIR = src

# Compiler flags
CFLAGS = -Wall $(INC)

#
# Project files
#
COMMONSRCS = $(SRC_DIR)/Tuple.cpp $(SRC_DIR)/Request.cpp
CLIENTSRCS = $(COMMONSRCS) $(SRC_DIR)/client.cpp
SERVERSRCS = $(COMMONSRCS) $(SRC_DIR)/server.cpp

#
# Object files
#
CLIENTOBJS = $(CLIENTSRCS:.cpp=.o)
SERVEROBJS = $(SERVERSRCS:.cpp=.o)
CLIENTEXE  = client
SERVEREXE = server

#
# Debug build settings
#
DBGDIR = debug
DBGEXECLIENT = $(DBGDIR)/$(CLIENTEXE)
DBGEXESERVER = $(DBGDIR)/$(SERVEREXE)
DBGOBJSCLIENT = $(addprefix $(DBGDIR)/, $(CLIENTOBJS))
DBGOBJSSERVER = $(addprefix $(DBGDIR)/, $(SERVEROBJS))
DBGCFLAGS = -g -O0 -DDEBUG

#
# Release build settings
#
RELDIR = release
RELEXECLIENT = $(RELDIR)/$(CLIENTEXE)
RELEXESERVER = $(RELDIR)/$(SERVEREXE)
RELOBJSCLIENT = $(addprefix $(RELDIR)/, $(CLIENTOBJS))
RELOBJSSERVER = $(addprefix $(RELDIR)/, $(SERVEROBJS))
RELCFLAGS = -O3 -DNDEBUG

.PHONY: all clean debug prep release remake

# Default build
all: prep release

#
# Debug rules
#
debug: $(DBGEXECLIENT) $(DBGEXESERVER)

$(DBGEXECLIENT): $(DBGOBJSCLIENT)
	$(CC) $(CFLAGS) $(DBGCFLAGS) -o $(DBGEXECLIENT) $^

$(DBGEXESERVER): $(DBGOBJSSERVER)
	$(CC) $(CFLAGS) $(DBGCFLAGS) -o $(DBGEXESERVER) $^

$(DBGDIR)/%.o: %.cpp
	$(CC) -c $(CFLAGS) $(DBGCFLAGS) -o $@ $<

#
# Release rules
#
release: $(RELEXECLIENT) $(RELEXESERVER)

$(RELEXECLIENT): $(RELOBJSCLIENT)
	$(CC) $(CFLAGS) $(RELCFLAGS) -o $(RELEXECLIENT) $^

$(RELEXESERVER): $(RELOBJSSERVER)
	$(CC) $(CFLAGS) $(RELCFLAGS) -o $(RELEXESERVER) $^

$(RELDIR)/%.o: %.cpp
	$(CC) -c $(CFLAGS) $(RELCFLAGS) -o $@ $<

#
# Other rules
#
prep:
	@mkdir -p $(DBGDIR) $(RELDIR) $(DBGDIR)/src $(RELDIR)/src

remake: clean all

clean:
	rm -f $(RELOBJSCLIENT) $(RELOBJSSERVER) $(RELEXECLIENT) $(RELEXESERVER) $(DBGOBJSCLIENT) $(DBGOBJSSERVER) $(DBGEXECLIENT) $(DBGEXESERVER)
	
#
# DreamDaq Makefile
#
# Version 2014 25.11.2014 roberto.ferrari@pv.infn.it

BINDIR          := $(HOME)/bin/

CXX_SRCS=$(wildcard *.cxx)
CXX_BINS=$(CXX_SRCS:%.cxx=%)

C_SRCS=$(wildcard *.c)
C_BINS=$(C_SRCS:%.c=%)
SCRIPTS=$(wildcard *Daq.sh)

all: $(CXX_BINS) $(C_BINS)
	@echo "****************************************************"
	@echo "        all done with g++ version `g++ -dumpversion`"
	@echo "****************************************************"

% : %.cxx
	g++ -g -O3 -o $@ $< -lCAENVME

% : %.c
	gcc -g -O3 -o $@ $< -lCAENVME

install :
	cp -p $(CXX_BINS) $(BINDIR)
	cp -p $(C_BINS) $(BINDIR)
	cp -p $(SCRIPTS) $(BINDIR)

clean:
	rm -f p $(CXX_BINS) $(C_BINS)


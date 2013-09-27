#
#*********************************************************************************************************
#                                                 uC/OS-II
#                                          The Real-Time Kernel
#                                                 PORT LINUX
#                                      MASTER INCLUDE FILE
#
#            Jerome CHEVALIER, Ecole Polytechnique de Montreal, Qc, CANADA
#                                                       09/2001
#
# File : Makefile
# By   : Jerome CHEVALIER	jerome.chevalier@polymtl.ca
# And the participation from :  Karim YAGHMOUR     yaghmourkarym@opersys.com  
#********************************************************************************************************
#


#
# Makefile for MicroC OS II emulated on Linux
#

# Definitions
CC         = gcc
DEBUG      = -g
DEFFLAGS   =
CFLAGS     = -O2 $(DEBUG) $(DEFFLAGS) -Wall
LFLAGS     = $(DEBUG) -static

# Files needed to build project
OBJS = tp.o ucos_ii.o os_cpu_c.o

# Generated files
PROJECT_EXEC   = tp

# Rules
all: tp

.depend:
	@echo "Building dependencies"
	rm -rf .depend
	touch .depend
	makedepend -f .depend $(DEFFLAGS) *.c
	@echo "Dependencies built"

depend dep: .depend

%.o: %.c
	$(CC) $(CFLAGS) -c $<

tp: ${OBJS}
	$(CC) $(LFLAGS) -o $(PROJECT_EXEC) ${OBJS}

clean:
	rm -rf *.o
	rm -rf $(PROJECT_NAME)

distclean:
	rm -rf *~
	rm -rf .depend .depend.bak
	rm -rf *.o $(PROJECT_EXEC) core

-include .depend

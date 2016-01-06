### Makefile.
# Wednesday, 21th December 2015.
# Author: {nettoclaudio,jvitor}@dcc.ufrj.br

## Options to compilation
# Prefered compiler
CC=/usr/bin/gcc
# Otimization level
CO=-O2
# Length of architecture
CM=-m64
# Flags to compilation
CFLAGS=-Wall -g

## Directories to source and binary code
PATH_BIN=./bin
PATH_SRC=./src

# Project's final compilation
all: dirs main

# Create directories
dirs:
	@mkdir -p $(PATH_BIN)

# Compiling the application
main: rpn numint log
	$(CC) $(CO) $(CM) -o $(PATH_BIN)/numint $(PATH_SRC)/Main.c $(PATH_BIN)/numint.o $(PATH_BIN)/rpn.o $(PATH_BIN)/log.o $(CFLAGS) -lpthread -lm
	@ln -s -f $(PATH_BIN)/numint ./
	@rm -f $(PATH_BIN)/*.o

# Compiling module to Numerical Integration
numint: rpn log
	$(CC) $(CO) $(CM) -c -o $(PATH_BIN)/numint.o $(PATH_SRC)/numerical_integration/numint.c $(CFLAGS) -lpthread -lm

# Compiling module to Reverse Polish Notation
rpn:  log
	$(CC) $(CO) $(CM) -c -o $(PATH_BIN)/rpn.o $(PATH_SRC)/rpn/reverse_polish_notation.c $(CFLAGS) -lm

log:
	$(CC) $(CO) $(CM) -c -o $(PATH_BIN)/log.o $(PATH_SRC)/utils/log.c $(CFLAGS)

clean:
	@rm -rf $(PATH_BIN)

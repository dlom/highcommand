CC = gcc
CC_FLAGS = -Wall -std=gnu99 -pedantic -g
LD_FLAGS =

.PHONY: all clean

all: test

test: test.c highcommand.h highcommand.o
	$(CC) $(CC_FLAGS) test.c highcommand.o -o test $(LD_FLAGS)

highcommand.o: highcommand.c highcommand.h highcommand_private.h
	$(CC) $(CC_FLAGS) -c highcommand.c -o highcommand.o $(LD_FLAGS)

clean:
	rm -f highcommand.o test

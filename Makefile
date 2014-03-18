CC = gcc
FLAGS = -Wall -std=gnu99 -pedantic -DHIGH_COMMAND_TEST -g
V_FLAGS = --leak-check=full --track-origins=yes

.PHONY: all clean valgrind test

all: highcommand-test

highcommand-test: highcommand.c highcommand.h testhelp.h
	$(CC) $(FLAGS) highcommand.c -o highcommand-test

clean:
	rm -f highcommand-test

test: highcommand-test
	./highcommand-test

valgrind: highcommand-test
	valgrind $(V_FLAGS) ./highcommand-test

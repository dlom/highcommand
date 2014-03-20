CC = gcc
FLAGS = -Wall -std=gnu99 -pedantic -DHIGH_COMMAND_TEST -g
NO_TEST = -DQUIET_TESTS
V_FLAGS = --leak-check=full --track-origins=yes

.PHONY: all clean valgrind test no-test

all: highcommand-test

highcommand-test: highcommand.c highcommand.h testhelp.h
	$(CC) $(FLAGS) highcommand.c -o highcommand-test

highcommand-no-test: highcommand.c highcommand.h testhelp.h
	$(CC) $(FLAGS) $(NO_TEST) highcommand.c -o highcommand-no-test

clean:
	rm -f highcommand-test
	rm -f highcommand-no-test

test: highcommand-test
	./highcommand-test

no-test: highcommand-no-test
	./highcommand-no-test

valgrind: highcommand-test
	valgrind $(V_FLAGS) ./highcommand-test

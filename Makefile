CC = clang
FLAGS = -g -Wall -DHIGH_COMMAND_TEST
V_FLAGS = --leak-check=full --track-origins=yes

.PHONY: all clean valgrind

all: highcommand-test

highcommand-test: highcommand.c highcommand.h testhelp.h
	$(CC) $(FLAGS) highcommand.c -o highcommand-test

clean:
	rm -f highcommand-test

valgrind: highcommand-test
	valgrind $(V_FLAGS) ./highcommand-test

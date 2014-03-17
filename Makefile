CC = clang
FLAGS = -g -Wall -DHIGH_COMMAND_TEST

.PHONY: all clean

all: highcommand-test
	./highcommand-test

highcommand-test: highcommand.c highcommand.h testhelp.h
	$(CC) $(FLAGS) highcommand.c -o highcommand-test

clean:
	rm -f highcommand-test

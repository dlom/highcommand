CC = gcc
CC_FLAGS = -Wall -std=gnu99 -pedantic -g
LD_FLAGS =

.PHONY: all clean

all: test

test: test.c hc.h hc.o
	$(CC) $(CC_FLAGS) test.c hc.o -o test $(LD_FLAGS)

hc.o: hc.c hc.h hc_private.h
	$(CC) $(CC_FLAGS) -c hc.c -o hc.o $(LD_FLAGS)

clean:
	rm -f hc.o test

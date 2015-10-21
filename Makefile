CC = gcc
CC_FLAGS = -Wall -std=gnu99 -pedantic -g

.PHONY: all clean lib

all: test

test: test.c hc.h hc.o
	$(CC) $(CC_FLAGS) test.c hc.o -o test

hc.o: hc.c hc.h hc_private.h
	$(CC) $(CC_FLAGS) -fPIC -c hc.c -o hc.o

libhc.so: hc.o
	$(CC) $(CC_FLAGS) -fPIC hc.o -o libhc.so -shared

lib: libhc.so

clean:
	rm -f hc.o libhc.so test

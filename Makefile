PREFIX    ?= /usr
LIBDIR     = $(PREFIX)/lib
INCLUDEDIR = $(PREFIX)/include

CC = gcc
CC_FLAGS = -Wall -Wextra -std=gnu99 -pedantic

.PHONY: all install clean

all: libhc.a

hc.o: hc.c hc.h hc_private.h
	$(CC) $(CC_FLAGS) -c hc.c -o hc.o

libhc.a: hc.o
	ar rcs libhc.a hc.o

install: libhc.a hc.h
	install -Dm644 libhc.a $(DESTDIR)$(LIBDIR)/libhc.a
	install -Dm644 hc.h $(DESTDIR)$(INCLUDEDIR)/hc.h

clean:
	rm -f hc.o libhc.a

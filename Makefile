PREFIX    ?= /usr
LIBDIR     = $(PREFIX)/lib
INCLUDEDIR = $(PREFIX)/include
MANDIR     = $(PREFIX)/

CC = gcc
CC_FLAGS = -Wall -Wextra -std=gnu99 -pedantic

.PHONY: all install clean man

all: libhc.a man/highcommand.3

hc.o: hc.c hc.h hc_private.h
	$(CC) $(CC_FLAGS) -c hc.c -o hc.o

libhc.a: hc.o
	ar rcs libhc.a hc.o

install: libhc.a hc.h hc.h man/highcommand.3
	install -Dm644 libhc.a $(DESTDIR)$(LIBDIR)/libhc.a
	install -Dm644 hc.h $(DESTDIR)$(INCLUDEDIR)/hc.h

man: man/highcommand.3

man/highcommand.3: man/highcommand.3.ronn
	ronn --manual="High Command" --organization="Mark Old" --roff man/highcommand.3.ronn

clean:
	rm -f hc.o libhc.a man/highcommand.3

VERSION_MAJOR = 1
VERSION_MINOR = 0
VERSION_PATCH = 0

VERSION = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)

PREFIX    ?= /usr
LIBDIR     = $(PREFIX)/lib
INCLUDEDIR = $(PREFIX)/include

CC = gcc
CC_FLAGS = -Wall -Wextra -std=gnu99 -pedantic

.PHONY: all install clean

all: libhc.so

hc.o: hc.c hc.h hc_private.h
	$(CC) $(CC_FLAGS) -fPIC -c hc.c -o hc.o

libhc.so: hc.o
	$(CC) $(CC_FLAGS) -fPIC -shared hc.o -o libhc.so

install: libhc.so hc.h
	install -Dm755 libhc.so $(DESTDIR)$(LIBDIR)/libhc.so.$(VERSION)
	install -Dm644 hc.h $(DESTDIR)$(INCLUDEDIR)/hc.h
	cd $(DESTDIR)$(LIBDIR) && ln -s libhc.so.$(VERSION) libhc.so.$(VERSION_MAJOR)
	cd $(DESTDIR)$(LIBDIR) && ln -s libhc.so.$(VERSION) libhc.so

clean:
	rm -f hc.o libhc.so

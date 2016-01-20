PREFIX    ?= /usr
LIBDIR     = $(PREFIX)/lib
INCLUDEDIR = $(PREFIX)/include
MANDIR     = $(PREFIX)/share/man/man3

CC         = gcc
CFLAGS    += -Wall -Wextra -std=gnu99 -pedantic

INSTALL   ?= install

FUNCTIONS = hc_opt hc_run hc_get_results hc_cleanup hc_is_present hc_has_value hc_get_value hc_get_level
MANPAGES = $(addprefix man/,$(addsuffix .3,$(FUNCTIONS)))

TESTS = $(basename $(wildcard tests/*_test.c))
TEST_OUTPUT = TAP

.PHONY: all install clean doc test check

all: libhc.a doc

hc.o: hc.c hc.h hc_private.h
	$(CC) $(CFLAGS) -c hc.c -o hc.o

libhc.a: hc.o
	$(AR) rcs libhc.a hc.o

install: libhc.a hc.h hc.h man/highcommand.3 $(MANPAGES)
	$(INSTALL) -Dm644 libhc.a $(DESTDIR)$(LIBDIR)/libhc.a
	$(INSTALL) -Dm644 hc.h $(DESTDIR)$(INCLUDEDIR)/hc.h
	$(INSTALL) -Dm644 man/highcommand.3 $(DESTDIR)$(MANDIR)/highcommand.3
	$(INSTALL)  -m644 $(MANPAGES) $(DESTDIR)$(MANDIR)

doc: man/highcommand.3 $(MANPAGES)

man/highcommand.3: man/highcommand.3.ronn
	ronn --manual="High Command" --organization="Mark Old" --roff man/highcommand.3.ronn

man/%.3:
	echo ".so man3/highcommand.3" > $@

test: $(TESTS)
	echo $^ | CMOCKA_MESSAGE_OUTPUT=$(TEST_OUTPUT) xargs -n 1 sh -c

tests/%_test: tests/%_test.c hc.o hc.h hc_private.h
	$(CC) $(CFLAGS) $< hc.o -o $@ -lcmocka

check: test

clean:
	$(RM) hc.o libhc.a
	$(RM) man/highcommand.3 $(MANPAGES)
	$(RM) $(TESTS)

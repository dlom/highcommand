PREFIX    ?= /usr
LIBDIR     = $(PREFIX)/lib
INCLUDEDIR = $(PREFIX)/include
MANDIR     = $(PREFIX)/share/man/man3

FUNCTIONS = hc_opt hc_run hc_get_results hc_cleanup hc_is_present hc_has_value hc_get_value hc_get_level
MANPAGES = $(addprefix man/,$(addsuffix .3,$(FUNCTIONS)))

CC = gcc
CC_FLAGS = -Wall -Wextra -std=gnu99 -pedantic

.PHONY: all install clean man

all: libhc.a man/highcommand.3

hc.o: hc.c hc.h hc_private.h
	$(CC) $(CC_FLAGS) -c hc.c -o hc.o

libhc.a: hc.o
	ar rcs libhc.a hc.o

install: libhc.a hc.h hc.h man/highcommand.3 $(MANPAGES)
	install -Dm644 libhc.a $(DESTDIR)$(LIBDIR)/libhc.a
	install -Dm644 hc.h $(DESTDIR)$(INCLUDEDIR)/hc.h
	install -Dm644 man/highcommand.3 $(DESTDIR)$(MANDIR)/highcommand.3
	install  -m644 $(MANPAGES) $(DESTDIR)$(MANDIR)

man: man/highcommand.3 $(MANPAGES)

man/highcommand.3: man/highcommand.3.ronn
	ronn --manual="High Command" --organization="Mark Old" --roff man/highcommand.3.ronn

man/%.3:
	echo ".so man3/highcommand.3" > $@

clean:
	rm -f hc.o libhc.a man/highcommand.3

CC = gcc
EXE = wol
CFLAGS = -Wall -O0 -c -g -I../include
LDFLAGS = -Wall
VPATH = ../include
PERL = /usr/bin/perl
LIBDIR = ../lib
INSTALLDIR = /usr/local/bin
OBJS = wol.o usage.o main.o

$(EXE): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

wol.o: wol.h common.h

usage.o: usage.h common.h

secure.o: secure.h

main.o: wol.h usage.h common.h

.PHONY: clean

clean:
	-rm -f $(OBJS)

install:
	mv $(EXE) $(INSTALLDIR)/$(EXE)

uninstall:
	rm -f $(INSTALLDIR)/$(EXE)

test:
	$(PERL) ../test/run_tests.pl

.c.o :
	$(CC) $(CFLAGS) -o $@ $<
CC = /usr/bin/gcc
EXE = wol
VPATH = ../include
PERL = /usr/bin/perl
LIBDIR = ../lib
INCDIR = ../include
CFLAGS = -Wall -O0 -c -g -I $(INCDIR)
LDFLAGS = -L $(LIBDIR)
INSTALLDIR = /usr/local/bin
OBJS = wol.o usage.o main.o

# main rule - generates executable
$(EXE): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

# generate build objects
wol.o: wol.h common.h

usage.o: usage.h common.h

secure.o: secure.h

main.o: wol.h usage.h common.h

.PHONY: clean install uninstall test

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
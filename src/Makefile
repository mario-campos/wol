CC = gcc
EXE = wol
CFLAGS = -Wall -O0 -c -g
LFLAGS = -Wall
INSTALLDIR = /usr/local/bin
OBJS = wol.o usage.o main.o

$(EXE): $(OBJS)
	$(CC) $(LFLAGS) -o $@ $(OBJS)

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

.c.o :
	$(CC) $(CFLAGS) -o $@ $<
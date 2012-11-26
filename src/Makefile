CC = gcc
EXE = wol
CFLAGS = -Wall -O0 -c -g
LFLAGS = -Wall
INSTALLDIR = /usr/local/bin
OBJS = wol.o usage.o main.o

$(EXE): $(OBJS)
	$(CC) $(LFLAGS) -o $@ $(OBJS)

wol.o: wol.c wol.h common.h

usage.o: usage.c usage.h common.h

secure.o: secure.c secure.h

main.o: main.c wol.h usage.h common.h

.PHONY: clean

clean:
	-rm -f $(OBJS)

install:
	mv $(EXE) $(INSTALLDIR)/$(EXE)

uninstall:
	rm -f $(INSTALLDIR)/$(EXE)

.c.o :
	$(CC) $(CFLAGS) -o $@ $<
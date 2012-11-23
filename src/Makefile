CC = gcc
CFLAGS = -Wall -O0 -c
LFLAGS = -Wall
DEBUG = -g
INSTALLDIR = /usr/local/bin
OBJS = wol.o usage.o main.o
OBJS-DBG = wol-dbg.o usage-dbg.o main-dbg.o

wol: $(OBJS)
	$(CC) $(LFLAGS) -o wol $(OBJS)

wol.o: wol.c wol.h common.h
	$(CC) $(CFLAGS) wol.c

wol-dbg.o: wol.c wol.h common.h
	$(CC) $(DEBUG) $(CFLAGS) wol.c

usage.o: usage.c usage.h common.h
	$(CC) $(CFLAGS) usage.c

usage-dbg.o: usage.c usage.h common.h
	$(CC) $(DEBUG) $(CFLAGS) usage.c

secure.o: secure.c secure.h
	$(CC) $(CFLAGS) secure.c

main.o: main.c wol.h usage.h common.h
	$(CC) $(CFLAGS) main.c

main-dbg.o: main.c wol.h usage.h common.h
	$(CC) $(DEBUG) $(CFLAGS) main.c

clean:
	rm -f *.o

debug: $(OBJS-DBG)
	$(CC) $(LFLAGS) -o wol-dbg $(OBJS)

install:
	mv wol $(INSTALLDIR)/wol

uninstall:
	rm -f $(INSTALLDIR)/wol
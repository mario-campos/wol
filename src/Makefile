CC = gcc
CFLAGS = -Wall -O0 -c
LFLAGS = -Wall
DEBUG = -g
INSTALLDIR = /usr/local/bin
OBJS = wol.o usage.o main.o

wol: $(OBJS)
	$(CC) $(LFLAGS) -o wol $(OBJS)

wol.o: wol.c wol.h common.h
	$(CC) $(CFLAGS) wol.c

usage.o: usage.c usage.h common.h
	$(CC) $(CFLAGS) usage.c

secure.o: secure.c secure.h
	$(CC) $(CFLAGS) secure.c

main.o: main.c wol.h usage.h common.h
	$(CC) $(CFLAGS) main.c

clean:
	rm -f *.o

debug:
	$(CC) $(DEBUG) $(CFLAGS) -o wol-dbg $(OBJS)

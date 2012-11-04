CC = gcc
CFLAGS = -Wall -O0 -c
LFLAGS = -Wall
DEBUG = -g
OBJS = wol.o usage.o main.o

wol: $(OBJS)
	$(CC) $(LFLAGS) -o wol $(OBJS)

wol.o: wol.c wol.h
	$(CC) $(CFLAGS) wol.c

usage.o: usage.c usage.h
	$(CC) $(CFLAGS) usage.c

secure.o: secure.c secure.h
	$(CC) $(CFLAGS) secure.c

main.o: main.c wol.h usage.h
	$(CC) $(CFLAGS) main.c

clean:
	rm -f *.o wol

debug:
	$(CC) $(DEBUG) $(CFLAGS) -o wol $(OBJS)
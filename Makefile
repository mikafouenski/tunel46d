CC = gcc

all: extremite

tunalloc.o:
	$(CC) -c tunalloc.c

extremite: tunalloc.o
	$(CC) -I. -o extermite tunalloc.o extremite.c

clean:
	rm -vf *.o
	rm -vf extremite

CC = gcc

all: tunalloc

extermite:
	$(CC) -o extermite extermite.c

tunalloc.o:
	$(CC) -c tunalloc.c

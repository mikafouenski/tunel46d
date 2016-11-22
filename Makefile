CC = gcc

all: tunalloc

tunalloc: tunalloc.c
	$(CC) -I. -o tunalloc -c tunalloc.c

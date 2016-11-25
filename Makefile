CC = gcc

all: tunalloc

tunalloc: tunalloc.c
	$(CC) -o tunalloc tunalloc.c

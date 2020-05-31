CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: emulate

emulate: assemble.o emulate.o
	$(CC) assemble.o emulate.o -o emulate


assemble.o: assemble.c
	$(CC) $(CFLAGS) -c assemble.c


emulate.o: emulate.c emulate.h structs.h constants.h instructionTypes.h
	$(CC) $(CFLAGS) -c emulate.c

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate

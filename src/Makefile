CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

assemble: assemble.o
	$(CC) $(CFLAGS) assemble.o -o assemble
emulate: emulate.o 
	$(CC) $(CFLAGS) emulate.o -o emulate
assemble.c: assemble.o:
	$(CC) -c assemble
emulate.c : emulate.o:
	$(CC) -c emulate
clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate

CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: emulate assemble

emulate: emulate.o
	$(CC) emulate.o -o emulate

assemble: assemble.o
	$(CC) assemble.o -o assemble

assemble.o: assembler/assemble.c assembler/assemble_utils.c assembler/assemble_utils.h
	$(CC) $(CFLAGS) -c assembler/assemble.c


emulate.o: emulator/emulate.c emulator/emulate.h emulator/machineState.h emulator/constants.h emulator/instructionTypes.h
	$(CC) $(CFLAGS) -c emulator/emulate.c

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate

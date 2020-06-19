# change application name here (executable output name)
TARGET=blm_quiz

# compiler
CC=gcc
# debug
DEBUG=-g
# optimisation
OPT=-O0
# warnings
WARN=-Wall

PTHREAD=-pthread

CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe

GTKLIB=`pkg-config --cflags --libs gtk+-3.0`

# linker
LD=gcc
LDFLAGS=$(PTHREAD) $(GTKLIB) -rdynamic

OBJS=    main.o

all: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)

main.o: src/main.c src/utils/multiple_choice.c src/utils/generate_tags.c src/utils/extension.h src/utils/pages.c src/utils/file_functions.c
	$(CC) -c $(CCFLAGS) src/main.c $(GTKLIB) -o main.o

clean:
	rm -f *.o $(TARGET)

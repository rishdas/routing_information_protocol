# Makefile template
# Patched by Lucas Brasilino

INCDIR=../include
DEPS = main.h proto.h
OBJ = main.o obj.o
CFLAGS=-I. -I$(INCDIR) -ggdb -gdwarf-2 
CC=gcc

all: $(OBJ)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


clean:
	rm -f *.o *~ 

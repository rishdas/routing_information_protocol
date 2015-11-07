# Makefile template
# Patched by Lucas Brasilino

INCDIR=./include
SRCDIR=./src
PROG = rip
CFLAGS=-I. -I$(INCDIR) -ggdb -gdwarf-2 
CC=gcc

all: $(PROG)

$(PROG):
	make -C src
	gcc -o $@ $^ $(SRCDIR)/*.o $(CFLAGS) $(LDFLAGS)

clean:
	make -C src clean
	rm -f *.o *~ $(PROG)

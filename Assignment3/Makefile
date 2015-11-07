# Makefile template
# Patched by Lucas Brasilino

INCDIR=./include
SRCDIR=./src
PROG = rip
CFLAGS=-I. -I$(INCDIR) -ggdb -gdwarf-2 
CC=gcc

all: $(PROG)

#%.o: $(SRCDIR)/%.c $(DEPS)
#	$(CC) -c -o $(SRCDIR)/$@ $< $(CFLAGS)

$(PROG):
	make -C src
	gcc -o $@ $^ $(SRCDIR)/*.o $(CFLAGS) $(LDFLAGS)

clean:
	rm -f *.o *~ $(PROG) $(SRCDIR)/*.o $(SRCDIR)/*~

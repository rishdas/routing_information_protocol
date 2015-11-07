# Makefile template
# Patched by Lucas Brasilino

INCDIR=../include
DEPS = rip_main.h rip_proto.h
OBJ = rip_main.o rip_obj.o rip_util.o
CFLAGS=-I. -I$(INCDIR) -ggdb -gdwarf-2 
CC=gcc

all: $(OBJ)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


clean:
	rm -f *.o *~ 

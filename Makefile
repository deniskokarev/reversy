#CFLAGS=-g -Wall
CFLAGS=-O3 -Wall

PROG=reversy
LIB=libreversy.a
OBJS=game.o minimax.o

all: $(PROG)

#.c.o:
#	$(CC) $(CFLAGS) -c -o $@ $<

$(PROG): main.o $(LIB)
	cc -o $(PROG) main.o $(LIB)

$(LIB): $(OBJS)
	ar -r $(LIB) $(OBJS)
	ranlib $(LIB)

clean:
	rm -f core *.o $(PROG) $(LIB) *~

game.o: game.h

minimax.o: game.h minimax.h

main.o: game.h minimax.h

CFLAGS=-g -Wall

PROG=reversy
OBJS=game.o minimax.o main.o

all: $(PROG)

#.c.o:
#	$(CC) $(CFLAGS) -c -o $@ $<

$(PROG): $(OBJS)
	cc -o $(PROG) $(OBJS)

clean:
	rm -f core *.o $(PROG) *~

game.o: game.h

minimax.o: game.h minimax.h

main.o: game.h minimax.h

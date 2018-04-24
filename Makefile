CFLAGS = -g -Wall -pg -lm -std=c89

calc: calc.c dstack.o cstack.o vars.o

dstack.o: dstack.c dstack.h

cstack.o: cstack.c cstack.h

vars.o: vars.c vars.h

clean:
	rm -f calc dstack.o cstack.o

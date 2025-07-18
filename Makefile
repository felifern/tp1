CFLAGS=-std=c99 -pedantic -Wall -Werror -c -g
LFLAGS=-lm

all:calculadora

calculadora: main.o racional.o entero.o pila.o cola.o 
	gcc main.o racional.o entero.o pila.o cola.o -o calculadora $(LFLAGS)

main.o: main.c racional.h entero.h 
	gcc main.c $(CFLAGS)

cola.o: cola.c cola.h
	gcc cola.c $(CFLAGS)


pila.o: pila.c pila.h
	gcc pila.c $(CFLAGS)

racional.o: racional.c racional.h entero.h
	gcc racional.c $(CFLAGS)

entero.o: entero.c entero.h
	gcc entero.c $(CFLAGS)

clean:
	rm *.o calculadora
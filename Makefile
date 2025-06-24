CFLAGS=-std=c99 -pedantic -Wall -Werror -c
LFLAGS=-lm

all:programa

programa: main.o racional.o entero.o pila.o cola.o operador.o
	gcc main.o racional.o entero.o pila.o cola.o operador.o -o programa $(LFLAGS)

main.o: main.c racional.h entero.h 
	gcc main.c $(CFLAGS)

cola.o: cola.c cola.h
	gcc cola.c $(CFLAGS)

operador.o: operador.c operador.h racional.h
	gcc operador.c $(CFLAGS)

pila.o: pila.c pila.h
	gcc pila.c $(CFLAGS)

racional.o: racional.c racional.h entero.h
	gcc racional.c $(CFLAGS)

entero.o: entero.c entero.h
	gcc entero.c $(CFLAGS)

clean:
	rm *.o programa
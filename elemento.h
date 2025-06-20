#ifndef ELEMENTO_H
#define ELEMENTO_H

enum tipo {OPERADOR, NUMERO, FUNCION, PARENTESIS};
typedef enum tipo tipo;

typedef struct {
    char *elemento;
    tipo tipo;
}elemento_t; 


void elemento_destruir(elemento_t*elemento);


/*
ok, correccion semi definitiva, no vamos a poner el puntero void aca. esto porque como de todas formas tenemos que
hacer una lista con los operadores y funciones que contengan los punteros es al pedo.
*/





#endif
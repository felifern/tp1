#ifndef ELEMENTO_H
#define ELEMENTO_H

enum tipo {OPERADOR, NUMERO, FUNCION};
typedef enum tipo tipo;

typedef struct {
    char *elemento;
    tipo tipo;
}elemento_t; 
/*
ok, correccion semi definitiva, no vamos a poner el puntero void aca. esto porque como de todas formas tenemos que
hacer una lista con los operadores y funciones que contengan los punteros es al pedo.
*/





#endif
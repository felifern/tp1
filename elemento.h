#ifndef ELEMENTO_H
#define ELEMENTO_H

enum tipo {OPERADOR, NUMERO, FUNCION};
typedef enum tipo tipo;

typedef struct {
    char *elemento;
    tipo tipo;
    //void *funcion;
}elemento_t; //Si agrego void *funcion deberia hacerse algun wrapper para poder englobar todo. (creo)
/*
de todas formas si hay que implementar una lista con los punteros a funciones 
no tiene mucho sentido agregarlo aca en el struct. CREOOOOOO hay que pensar con la cabezaaaaaaaaaaaa.

pero es buena la idea.
*/





#endif
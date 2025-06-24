#ifndef OPERADOR_H
#define OPERADOR_H

#include "racional.h"
#include <string.h>
#include <stdio.h>


typedef struct{
    char *operador;
    racional_t *(*funcion) (const racional_t *a, const racional_t *b);//podemos hacero esto.
    int aridad;
    int prioridad;
    char *descripcion;
}operador_t;



//nueva idea : 3 tablas (aridad 0,1,2)
typedef racional_t *(*op_binaria_t)(racional_t *, racional_t *);
typedef racional_t *(*op_unaria_t)(racional_t *);
typedef racional_t *(*op_cero_t)(void);

struct operador_dos {
    const char *nombre;
    op_binaria_t funcion;
    int prioridad;
};

struct operador_uno {
    const char *nombre;
    op_unaria_t funcion;
    int prioridad;
};

struct operador_cero{
    const char *nombre;
    op_cero_t funcion;
    int prioridad;
};

op_binaria_t buscar_operador_dos(const char *nombre);
op_unaria_t buscar_operador_uno(const char *nombre);
op_cero_t buscar_operador_cero(const char *nombre);




// bueno, se usa lo primero, no borro esto porque capaz sirve de inspiracion para alguna otra cosa.
// y hasta arreglar las funciones que lo usaban marca errores.

#endif
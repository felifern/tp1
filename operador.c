#include "operador.h"
#include "racional.h"
#include <string.h>








/*

typedef struct operador_dos operador_dos_t;
typedef struct operador_uno operador_uno_t;
typedef struct operador_cero operador_cero_t;

operador_dos_t operadores_dos[] = {
    {"+", racional_sumar,1},
    {"-", racional_restar,1},
    {"*", racional_multiplicar,2},
    {"/", racional_dividir,2},
    //{"^", racional_potencia,4} (falta implementar lpm)
    {NULL, NULL, 0}
};

operador_uno_t operadores_uno[] = {
    {"_",racional_inverso, 5},
    {"fact", racional_factorial,},
    {"inv",racional_inverso_multiplicativo,},
    {NULL, NULL, 0}
};

operador_cero_t operadores_cero[] = {
    {"pi", racional_pi},
    {"e", racional_e},
    {"phi",racional_phi},
    {NULL, NULL, 0}
};

op_binaria_t buscar_operador_dos(const char *nombre) {
    for (int i = 0; operadores_dos[i].nombre != NULL; i++) {
        if (strcmp(nombre, operadores_dos[i].nombre) == 0)
            return operadores_dos[i].funcion;
    }
    return NULL;
}

op_unaria_t buscar_operador_uno(const char *nombre) {
    for (int i = 0; operadores_uno[i].nombre != NULL; i++) {
        if (strcmp(nombre, operadores_uno[i].nombre) == 0)
            return operadores_uno[i].funcion;
    }
    return NULL;
}

op_cero_t buscar_operador_cero(const char *nombre) {
    for (int i = 0; operadores_cero[i].nombre != NULL; i++) {
        if (strcmp(nombre, operadores_cero[i].nombre) == 0)
            return operadores_cero[i].funcion;
    }
    return NULL;
}*/
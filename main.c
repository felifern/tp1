#include <stdio.h>
#include "entero.h"
#include "pila.h"
#include "racional.h"
#include "cola.h"



cola_t *leer_linea(); // lee la linea y la guarda en una cola. ej: {"123", "+", "2", "*", "5"} devuelve la cant de elementos por n.

cola_t *pasar_a_postfija(cola_t *infija); //

racional_t *operar_postfija(cola_t *polaca, racional_t *(*operar)(const racional_t*a, const racional_t *b)){
}

//Acá iría también la tabla de busqueda?? (o armamos un nuevo tipo para los operadores)



int main(){

    printf("pastel de papa");
    printf("pastel de papaAAAAA");

    return 0;
}

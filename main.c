#include <stdio.h>
#include "entero.h"
#include "pila.h"
#include "racional.h"
#include "cola.h"



cola_t *leer_linea(); // lee la linea y la guarda en una cola. ej: {"123", "+", "2", "*", "5", "+", "fact", "6"} (desactualizado pero es como la idea) devuelve la cant de elementos por n.

cola_t *pasar_a_postfija(cola_t *infija); //

racional_t *operar_postfija(cola_t *polaca /*,racional_t *(*operar)(const racional_t*a, const racional_t *b)*/){
    //la idea seria tipo plantear una cola con elementos_t los defino el el .h
    //si la cadena es isdigit, entonces es numero, si es is alpha entonces es funcion, 
    //si es cada caracter especifico entonces es  

}



int main(){

    printf("pastel de papa");
    printf("pastel de papaAAAAA");

    return 0;
}

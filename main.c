#include <stdio.h>
#include "entero.h"
#include "pila.h"
#include "racional.h"
#include "cola.h"
#include "lista.h"


typedef struct{
    char *operador;
    void (*funcion) (void);
    char *descripcion;
}operador; //Si agrego void *funcion deberia hacerse algun wrapper para poder englobar todo. (creo)
/*
el tema del wrapper es para ver como pasar del void * al puntero a la funcion que necesitamos usar.
pero no debe haber muchos tipos de funciones para definir. (en realidad no se, imagino que debe haber a los sumo 4 o 6)
*/

// entonces en la pila apilamos elemntos_t y si son operadores o funciones podemos buscarlos en al lista que nos
// obligan a hacer. 

// capaz hacer el elemento.h es innecesario, se puede definir todas esas estructuras aca. 
// queda mas desprolijo supongo, pero es mas rapido(?.



lista_t lista_crear();









cola_t *leer_linea(); // lee la linea y la guarda en una cola. ej: {"123", "+", "2", "*", "5", "+", "fact", "6"} (desactualizado pero es como la idea)


cola_t *pasar_a_postfija(cola_t *infija); //

racional_t *operar_postfija(cola_t *polaca /*,racional_t *(*operar)(const racional_t*a, const racional_t *b)*/){
    //la idea seria tipo plantear una cola con elementos_t los defino el el .h
    //si la cadena es isdigit, entonces es numero, si es is alpha entonces es funcion, 
    //si es cada caracter especifico entonces es operador.

}



int main(){

    printf("pastel de papa");
    printf("pastel de papaAAAAA");

    return 0;
}

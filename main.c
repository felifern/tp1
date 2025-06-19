#include <stdio.h>
#include "entero.h"
#include "pila.h"
#include "racional.h"
#include "cola.h"
#include "lista.h"
#include "operador.h"

//hay que hacer una funcion que me lea la linea de stdin y me devuelva un char* con esa linea (con fgets supongo?)
cola_t *lee_una_linea(){ //recibe linea "123+fact(5*3)" y guarda en una cola {"123","+","fact","(","5","*","3",")"}

    cola_t *cola = cola_crear();
    if (cola == NULL) return NULL;
    int c;
    char elemento[100]; //obviamente esta mal, habria que usar mem dinamica pero hay que ver como hacer eso
    //oooo podríamos hacer en la funcion que lea la linea, que me devuelva el numero de caracteres
    //de esa linea yyy entonces hacemos un malloc para el elemento con esa cantidad.
    //sería para asegurarnos que entre cada uno pero nada, tmp es muy eficiente (pedimos muucha memoria de mas en la mayoria de casos)
    //por ej "123*(545-234)" leo y devuelvo n=13 entonces hago q el tamaño max del elemento es 13.
    c = getchar();
    while(c != EOF && c != '\n') {
        
        if (c == ' '){
            c = getchar();
            continue;
        }
        if (isdigit(c) || c == '.'){
            int j = 1;
            elemento[0]= c;
            while (isdigit(c = getchar()) || c == '.'){ //como verifico que haya un solo punto en el numero?
                elemento[j++] = c;
            }
            //para hacerlo con mem dinamica habria que hacer dos whiles?? (medio (muy) ineficiente)
            //digo tipo contar los digitos y dsps hacer malloc...
            elemento[j] = '\0';
            cola_encolar(cola,elemento);
        }
        else if (isalpha(c)){
            int j = 1;
            elemento[0]= c;
            while (isalpha(c = getchar())){
                elemento[j++] = c;
            }
            elemento[j] = '\0';
            cola_encolar(cola,elemento);
        }
        else if (strchr("+-*/()^", c)){
            //encontre esa funcion así no hacemos mil OR's para cada simbolito válido
            elemento[0] = c;
            elemento[1] = '\0';
            cola_encolar(cola, elemento);
        }
        

    }
    return cola;
}




cola_t *pasar_a_postfija(cola_t *infija); //

racional_t *operar_postfija(cola_t *polaca){
    //la idea seria tipo plantear una cola con elementos_t los defino el el .h
    //si la cadena es isdigit, entonces es numero, si es is alpha entonces es funcion, 
    //si es una palabra que sea una funcion entonces es funcion.
}
void suma_wrpr (void **a){
    racional_t *resultado = racional_sumar((const racional_t *)a[0], (const racional_t *)a[1]);
    a[0] = resultado;
}



int main(){    
    // en el intento del ejemplo me di cuenta que faltan definir una banda de cosas (a lo ultimo digo algo al respecto)
    // lo del elemento.h esta bien, operador.h me genera dudas,
    // pero supongo que de alguna forma hay que hacer lo que nos piden

    // imaginamos que de la pila se desapilan "3", "2", "+" o como funcione el algoritmo, queremos sumar 2+3
    racional_t *a;//ponele que es el 2
    racional_t *b;// ponele que es el 3
    racional_t **elementos;// aca guardo el 2 y el 3

    operador_t suma;// asi deberiamos definir las cosas ponele
    suma.funcion = suma_wrpr;
    
    // desapilando los 2 tenemos (esto tenemos que definir, sino no se puede pensar esto) supone un puntero a punteros
    // racional_t entonces si yo paso eso a la funcion deberia funcionar y devolver por el primero el resultado

    elementos[0] = a; 
    elementos[1] = b;

    suma.funcion(elementos);
    //ahora el resultado estaria en elementos[0](es un ejemplo igual, no se si seria bueno implementar asi)
    racional_imprimir(elementos[0]);

    // realmente esto es una mierda, pero ahora hay algo con lo que preguntar. igual hay que ver que tan lejos esta
    // de ser una buena idea. 

    // lo que dije que falta definir una "banda" de cosas es mas que nada como vamos a tratar a los elementos que
    // vayamos apilando y manipulando. pero esto ultimo es mas que nada la logica de la funcion que opera todo esto,
    // tipo porque llegamos con los char* dependiendo del tipo que sean hacemos distintas cosas. (si son numeros
    // los pasamos a racional en tiempo de ejecucion y los volvemos a transformar a char*. si son operadores y
    // los tenemos que usar los buscamos y los usamos como propuse arriba). Y escribiendo esto me da un poco la 
    // pauta de que manejar esa memoria va a ser un quilombo y se deberian hacer muchos pases de tipos.

    // otra cosa con operador.h como tenemos que saber bien la firma de cada funcion tampoco habria alguna forma de 
    // agregar operaciones a traves de una funcion como si fuera un TDA. 
    // esto (el ej) lo hago bajo la asuncion de que deberiamos trabajar con cualquier tipo de funcion que se le pueda
    // ocurrir al tipo que vaya a usar el programa. 
    // ponele que todo esto se simplifica a funciones que reciban 0, 1 o 2 (3 a lo sumo) parametros de tipo racional_t y 
    // devuelvan racional_t ahi seria distinto, se podria hacer todo esto que estoy diciendo mucho mas facil
    // o eso creo. ademas, porque agregaria una funcion que no opere numeros, pero bueno hay que preguntar.


    // es medio choto esto la verdad, y no se cuanto de todo esto se entienda asi hablando por COMENTARIOS DE VSCODE,
    // tremendo, ademas no se me cae una tilde, tampoco se poneralas con el teclado en ingles srry.





    printf("pastel de papa");
    printf("pastel de papaAAAAA");

    return 0;
}

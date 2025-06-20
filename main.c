#include <stdio.h>
#include <string.h>
#include "entero.h"
#include "pila.h"
#include "racional.h"
#include "cola.h"
#include "lista.h"
#include "operador.h"
#include "elemento.h"





cola_t *lee_una_linea(){ //lee linea "123+fact(5*3)\n" y guarda en una cola {"123","+","fact","(","5","*","3",")"}

    cola_t *cola = cola_crear();
    if (cola == NULL) return NULL;
    int c;
    c = getchar();
    while(c != EOF && c != '\n') {
        
        if (c == ' '){
            c = getchar();
            continue;
        }
        if (isdigit(c) || c == '.'){
            int j = 1;
            char *num = malloc(sizeof(char) * 2);
            if(num == NULL){
                cola_destruir(cola, NULL);
                return NULL;
            }
            num[0] = c;
            size_t n_puntos = 0;
            if (c == '.') n_puntos++;
            c = getchar();

            while ((isdigit(c) || c == '.') && n_puntos < 2){
                if (c == '.') n_puntos++;
                char *aux = realloc(num, sizeof(char) * (j + 2));
                if (aux == NULL){
                    free(num);
                    cola_destruir(cola,NULL);
                    return NULL;
                }
                num = aux;
                num[j++] = c;
                c = getchar();
            }
            num[j] = '\0';

            elemento_t *elemento = malloc(sizeof(elemento_t));
            if (elemento == NULL){
                free(num);
                cola_destruir(cola, NULL);
                return NULL;
            }
            elemento->elemento = num;
            elemento->tipo = NUMERO;

            if (!cola_encolar(cola,elemento)){
                free(elemento->elemento);
                free (elemento);
                cola_destruir(cola,NULL);
                return NULL;
            }

            continue;
        }
        else if (isalpha(c)){
            int j = 1;
            char *fun = malloc(sizeof(char) * 2); 
            if(fun == NULL){
                cola_destruir(cola, NULL);
                return NULL;
            }
            fun[0] = c;
            c = getchar();
            while (isalpha(c)){
                char *aux = realloc(fun, sizeof(char) * (j + 2));
                if(aux == NULL) {
                    free(fun);
                    cola_destruir(cola, NULL);
                    return NULL;
                } 
                fun = aux;
                fun[j++] = c;
                c = getchar();
            }
            fun[j] = '\0';

            elemento_t *elemento = malloc(sizeof(elemento_t));
            if (elemento == NULL){
                free(fun);
                cola_destruir(cola, NULL);
                return NULL;
            }
            elemento->elemento = fun;
            elemento->tipo = FUNCION;
            if (!cola_encolar(cola, elemento)){
                free(elemento->elemento); 
                free(elemento);
                cola_destruir(cola, NULL);
                return NULL;
            }

            continue;
        }
        else if (c == ')' || c == '('){
            char *operador = malloc(sizeof(char) * 2); 
            if(operador == NULL){
                cola_destruir(cola, NULL);
                return NULL;
            }
            operador[0] = c;
            operador[1] = '\0';
            c =getchar();

            elemento_t *elemento = malloc(sizeof(elemento_t));
            if (elemento == NULL){
                free(operador);
                cola_destruir(cola, NULL);
                return NULL;
            }
            elemento->elemento = operador;
            elemento->tipo = PARENTESIS;
            if (!cola_encolar(cola, elemento)){
                free(elemento->elemento); 
                free(elemento);
                cola_destruir(cola, NULL);
                return NULL;
            }

            continue;
        }
        else if (strchr("+-*/^", c) != NULL && c != '\0'){ // esta funcion que encontraste devuelve un puntero a la primera ocurrencia de c en la linea esa, por eso dintinto de null tambien hay que chequear que sea distinto del '\0' era el comentario mas largo de la historia
            char *operador = malloc(sizeof(char) * 2); 
            if(operador == NULL){
                cola_destruir(cola, NULL);
                return NULL;
            }
            operador[0] = c;
            operador[1] = '\0';
            c =getchar();

            elemento_t *elemento = malloc(sizeof(elemento_t));
            if (elemento == NULL){
                free(operador);
                cola_destruir(cola, NULL);
                return NULL;
            }
            elemento->elemento = operador;
            elemento->tipo = OPERADOR;
            if (!cola_encolar(cola, elemento)){
                free(elemento->elemento); 
                free(elemento);
                cola_destruir(cola, NULL);
                return NULL;
            }

            continue;
        }
        else{
            c = getchar();
        }
    }
    return cola;
}
//suponer que esto esta bien, hay que probarlo igual



cola_t *pasar_a_postfija(cola_t *infija, operador_t **operadores, size_t oplen){// recibo una cola con los elementos_t
    bool parentesis_abierto = false;

    cola_t *salida = cola_crear();
    if(salida == NULL) return NULL;

    pila_t *auxiliar = pila_crear();
    if(auxiliar == NULL){
        cola_destruir(salida,elemento_destruir);//cola_destruir(salida,NULL);
        return NULL;
    }
    
    elemento_t *element;

    while ((element = cola_desencolar(infija)) != NULL){
        if(element->tipo == NUMERO){
            cola_encolar(salida, element);
            continue;
        }
        if(element->tipo == FUNCION || (element->tipo == PARENTESIS && element->elemento == "(")){
            //por qué incluis si el tipo es funcion?? eso no iria con la parte de operadores?
            pila_apilar(auxiliar, element);
            continue;
        }
        if (element->tipo == OPERADOR){// creo que aca hace falta ver el caso en el cual la pila este vacia (creo) 
            //sería if (tope == NULL) ... (libero y return NULL) (dsps de pila_ver_tope)
            elemento_t *tope = pila_ver_tope(auxiliar);
            if(buscar(operadores, 0, oplen -1,tope->elemento) >= buscar(operadores, 0, oplen -1,element->elemento)){ 
                while((tope = pila_desapilar(auxiliar))!= NULL){
                    cola_encolar(salida ,tope);
                }
                pila_apilar(auxiliar, element);
            }
            else{
                pila_apilar(auxiliar, element);
            }
            continue;
        }
        if(element->tipo == PARENTESIS && element->elemento == ")"){ // LOS PARENTESIS QUE TIPO SON? agrego un tipo, fue
            //creo q es bastante lo mismo si ponias element->tipo == OPERADOR
            //porque dsps comparas para ver cual es (")"). 
            //IGUAL creo que nos va a servir tener un tipo de elemento parentesis (probablemente no xd)
            elemento_t *tope = pila_desapilar(auxiliar);
            if (tope == NULL){
                printf("escribiste mal flaco");// no se
                // capaz hay que liberar memoria aca, ni idea
                //SÍ
                return NULL;
            }
            while(tope->elemento != "("){
                cola_encolar(salida ,tope);
                elemento_t *tope = pila_desapilar(auxiliar);
            }
            parentesis_abierto = true;//para qué sirve este booleano...
        
            continue;    
        }
    }

    elemento_t *tope = pila_ver_tope(auxiliar);
    while((tope = pila_desapilar(auxiliar))!= NULL){
        cola_encolar(salida ,tope);
    }
    //aca capaz hay que destruir un par de cosillas
    // no lo se la verdad.
    
    return salida;

}

racional_t *operar_postfija(cola_t *polaca){
    //la idea seria tipo plantear una cola con elementos_t los defino el el .h
    //si la cadena es isdigit, entonces es numero, si es is alpha entonces es funcion, 
    //si es una palabra que sea una funcion entonces es funcion.
}
void suma_wrpr (void **a){
    racional_t *resultado = racional_sumar((const racional_t *)a[0], (const racional_t *)a[1]);
    a[0] = resultado;
}


operador_t *operadores(size_t *n);// llena lista de los operadores (es para que quede mas prolijo)

int buscar(const operador_t *lista[], size_t izq, size_t der, char * x) { // esta la saque de una diapo, habria que ver si la modifique como corresponde
	if (izq > der) {
		return -1;
	}
    size_t medio = (izq + der) / 2;
    if (strcmp(lista[medio]->operador,x) < 0) {
        return lista[medio]->prioridad;
    }
    if (strcmp(lista[medio]->operador,x)<0) {
        return buscar(lista, izq, medio - 1, x);
    } else {
        return buscar(lista, medio + 1, der, x);
    }
}


int main(){    
    size_t oplen;
    operador_t *op = operadores(&oplen);

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

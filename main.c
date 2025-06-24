#include <stdio.h>
#include <string.h>
#include "entero.h"
#include "pila.h"
#include "racional.h"
#include "cola.h"
#include "lista.h"
#include "operador.h"
#include <stdlib.h>
#include <ctype.h>

//#include "elemento.h"

enum tipo {OPERADOR, NUMERO, FUNCION, PARENTESIS};
typedef enum tipo tipo;

typedef struct {
    char *elemento;
    tipo tipo;
}elemento_t; 

void elemento_destruir(elemento_t*elemento);

int buscar(operador_t **lista, size_t listlen , char * x) { // esta la saque de una diapo, habria que ver si la modifique como corresponde
	for (size_t i = 0; i<listlen; i++){
        if(strcmp(lista[i]->operador, x) == 0){
            return lista[i]->aridad;
        }
    }
    return -1;
}

cola_t *leer_linea(){ //lee linea "123+fact(5*3)\n" y guarda en una cola {"123","+","fact","(","5","*","3",")"}

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
        else if (strchr("+-*/^_", c) != NULL && c != '\0'){ // esta funcion que encontraste devuelve un puntero a la primera ocurrencia de c en la linea esa, por eso dintinto de null tambien hay que chequear que sea distinto del '\0' era el comentario mas largo de la historia
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


// esta funcion esta como el orto, despues la leo con mas calma. <------------------- YO ME OCUPO
cola_t *pasar_a_postfija(cola_t *infija, operador_t **operadores, size_t oplen){// recibo una cola con los elementos_t
    //bool parentesis_abierto = false;

    cola_t *salida = cola_crear();
    if(salida == NULL) return NULL;

    pila_t *auxiliar = pila_crear();
    if(auxiliar == NULL){
        //cola_destruir(salida,elemento_destruir);//cola_destruir(salida,NULL);
        return NULL;
    }
    elemento_t *tope;
    elemento_t *element;

    while ((element = cola_desencolar(infija)) != NULL){
        if(element->tipo == NUMERO){
            cola_encolar(salida, element);
            continue;
        }
        if(element->tipo == FUNCION || (element->tipo == PARENTESIS && strcmp(element->elemento, "(") == 0)){
            pila_apilar(auxiliar, element);
            printf("\napilo esto :%s\n", element->elemento);
            continue;
        }
        if (element->tipo == OPERADOR){// creo que aca hace falta ver el caso en el cual la pila este vacia (creo) 
            //sería if (tope == NULL) ... (libero y return NULL) (dsps de pila_ver_tope)
            // esto, hay que verlo
            tope = pila_ver_tope(auxiliar);
            if (tope == NULL || strcmp(tope->elemento, "(") == 0){
                pila_apilar(auxiliar, element);
                continue;
            }
            tope = pila_desapilar(auxiliar);
            while(buscar(operadores,oplen,tope->elemento) >= buscar(operadores,oplen ,element->elemento)||strcmp(tope->elemento, "(") != 0||(tope = pila_desapilar(auxiliar))!= NULL){
                cola_encolar(salida ,tope);
                tope = pila_desapilar(auxiliar);
            }
            pila_apilar(auxiliar, element);
            continue;
        }
        if(element->tipo == PARENTESIS && strcmp(element->elemento, ")") == 0){ // LOS PARENTESIS QUE TIPO SON? agrego un tipo, fue
            tope = pila_desapilar(auxiliar);
            if(tope == NULL){
                printf("escribiste mal flaco lo primero");
                continue;
            }
            while(tope != NULL && strcmp(tope->elemento, "(") != 0){
                cola_encolar(salida ,tope);
                printf("entra");
                printf("%s anterior a que falle", tope->elemento);
                tope = pila_desapilar(auxiliar);
            }
            if(tope == NULL){
                printf("escribiste mal flaco");
                continue;
            }
            else{//si entra aca es (
                tope = pila_desapilar(auxiliar);
            }
            //parentesis_abierto = true;//para qué sirve este booleano...
            //falta lo de: Si en el tope de la pila hay una función: Se pasa de la pila a la cola.
            // rta: no se que estas hablando
            continue;    
        }
    }

    tope = pila_ver_tope(auxiliar);
    while((tope = pila_desapilar(auxiliar))!= NULL){
        if(strcmp(tope->elemento,"(")){
            continue;
        }
        cola_encolar(salida ,tope);
    }
    //aca capaz hay que destruir un par de cosillas
    // no lo se la verdad.
    
    return salida;

}

/*
racional_t *operar_postfija(cola_t *polaca){ //llegamos aca con la notacion bien escrita (cualquier error se detecta cuando paso de infija a postfija)
    //la idea seria tipo plantear una cola con elementos_t los defino el el .h
    //si la cadena es isdigit, entonces es numero, si es is alpha entonces es funcion, 
    //si es una palabra que sea una funcion entonces es funcion.
    pila_t *pila = pila_crear(); //va a ser una pila con racionales_t
    if (pila == NULL){
        //libero la mem de polaca? (xq si el algoritmo funciona bien, terminas liberandola toda)
        cola_destruir(polaca,elemento_destruir);
        return NULL;
    }
    while (!cola_esta_vacia(polaca)){
        elemento_t *simbolo = cola_desencolar(polaca);

        if (simbolo->tipo == NUMERO){

            racional_t *numero;//falta implementar una funcion q pase de cadena a racional_t (o ver como hacerlo con las funciones q ya tenemos)
            if (!pila_apilar(pila, numero)){
                pila_destruir(pila, racional_destruir);
                cola_destruir(polaca,elemento_destruir);
            }
        }
        else if (simbolo->tipo == OPERADOR){
            //busco en la tabla de aridad 2, si no esta busco en la de 1, si no esta, escribió cualquier cosa
            op_binaria_t funcion_dos = buscar_operador_dos(simbolo->elemento);
            if (funcion_dos != NULL){
                racional_t *a = pila_desapilar(pila);
                if (a == NULL){
                    pila_destruir(pila, racional_destruir);
                    cola_destruir(polaca,elemento_destruir);
                    return NULL;
                }
                racional_t *b = pila_desapilar(pila);
                if (b == NULL){
                    pila_destruir(pila, racional_destruir);
                    cola_destruir(polaca,elemento_destruir);
                    racional_destruir(a);
                    return NULL;
                }
                //el primero q sale es el segundo parametro
                racional_t *resultado = funcion_dos(b,a);
                racional_destruir(a);
                racional_destruir(b);
                if (resultado == NULL){
                    cola_destruir(polaca,elemento_destruir);
                    pila_destruir(pila,racional_destruir);
                    return NULL;
                }
                pila_apilar(pila,resultado);
            }
            op_unaria_t funcion_uno = buscar_operador_uno(simbolo->elemento);
            if (funcion_uno != NULL){ 
                racional_t *num = pila_desapilar(pila);
                if (num == NULL){
                    pila_destruir(pila, racional_destruir);
                    cola_destruir(polaca,elemento_destruir);
                    return NULL;
                }
                racional_t *resultado = funcion_uno(num);
                racional_destruir(num);
                if (resultado == NULL){
                    pila_destruir(pila, racional_destruir);
                    cola_destruir(polaca,elemento_destruir);
                    return NULL;
                }
                pila_apilar(pila,resultado);
            }
            pila_destruir(pila,racional_destruir);
            cola_destruir(polaca,elemento_destruir);
        }
        else if (simbolo->tipo == FUNCION){
            //busco en la tabla de aridad 1, si no esta busco en la de 0, si no esta, escribió cualquier cosa
            op_unaria_t funcion_uno = buscar_operador_uno(simbolo->elemento);
            if (funcion_uno != NULL){ 
                racional_t *num = pila_desapilar(pila);
                if (num == NULL){
                    pila_destruir(pila, racional_destruir);
                    cola_destruir(polaca,elemento_destruir);
                    return NULL;
                }
                racional_t *resultado = funcion_uno(num);
                racional_destruir(num);
                if (resultado == NULL){
                    pila_destruir(pila, racional_destruir);
                    cola_destruir(polaca,elemento_destruir);
                    return NULL;
                }
                pila_apilar(pila,resultado);
            }
            op_cero_t funcion_cero = buscar_operador_cero(simbolo->elemento);
            if (funcion_cero != NULL){ 
                racional_t *resultado = funcion_cero();
                if (resultado == NULL){
                    pila_destruir(pila, racional_destruir);
                    cola_destruir(polaca,elemento_destruir);
                    return NULL;
                }
                pila_apilar(pila,resultado);
            }
            //si no entro en ningun if, no existe la funcion
            pila_destruir(pila,racional_destruir);
            cola_destruir(polaca,elemento_destruir);
            return NULL;
        }
    }
    //deberia quedar un solo elemento pero chequeamos por si las moscas ahre
    racional_t * resultado_final = pila_desapilar(pila);
    if (resultado_final == NULL || !pila_esta_vacia(pila)){
        pila_destruir(pila, racional_destruir);
        racional_destruir(resultado_final);
        //esta parte creo q esta mal xq si resultado_final == NULL y lo libero es un error
        cola_destruir(polaca,elemento_destruir);  
        return NULL;     
    }
    pila_destruir(pila,racional_destruir);
    return resultado_final;
    
}
*/

operador_t *operadores(size_t *n);// llena lista de los operadores (es para que quede mas prolijo)



operador_t *operador_crear(char *operador, racional_t *(*funcion) (const racional_t *a, const racional_t *b), int aridad,
int prioridad, char *descripcion){
    operador_t *nuevo = malloc(sizeof(operador_t));
    if (nuevo == NULL){
        return NULL;
    }
    char *nombre = malloc(strlen(operador) + 1);
    if(nombre == NULL){
        free(nuevo);
        return NULL;
    }
    memcpy(nombre, operador, strlen(operador) + 1);
    char *desc = malloc(strlen(descripcion) + 1);
    if(desc == NULL){
        free(nombre);
        free(nuevo);
        return NULL;
    }
    memcpy(desc, descripcion, strlen(descripcion) + 1);
    nuevo->operador = nombre;
    nuevo->funcion = funcion;
    nuevo->aridad = aridad;
    nuevo->prioridad = prioridad;
    nuevo->descripcion = descripcion;

    return nuevo;
}

operador_t **tabla_crear(size_t *n){
    operador_t **tabla = malloc(20 * sizeof(operador_t*));
    if(tabla == NULL){
        return NULL;
    }
    tabla[0] = operador_crear("suma", racional_sumar, 2, 1, "es una suma");
    tabla[1] = operador_crear("resta", racional_restar, 2, 1, "es una resta");
    tabla[2] = operador_crear("producto", racional_multiplicar, 2, 2, "es un producto");
    tabla[3] = operador_crear("division", racional_dividir, 2, 3, "es una division");
    //tabla[4] = operador_crear("potencia", racional_elevar, 2, 2, "es una potencia"); //pero hay que hacerla jj
    *n = 4;
    return tabla;
}





int main(){    
    /*
    size_t oplen;
    operador_t *op = operadores(&oplen);

    // imaginamos que de la pila se desapilan "3", "2", "+" o como funcione el algoritmo, queremos sumar 2+3
    racional_t *a;//ponele que es el 2
    racional_t *b;// ponele que es el 3
    racional_t **elementos;// aca guardo el 2 y el 3

    operador_t suma;
    suma.funcion = suma_wrpr;
    suma.funcion(a, b);
    racional_imprimir(elementos[0]);
    */
    size_t tablen;
    operador_t **tabla = tabla_crear(&tablen);
    
    cola_t *prueba;
    cola_t *prueba2;
    
    prueba = leer_linea();
    elemento_t *leo;

    prueba2 = pasar_a_postfija(prueba, tabla, tablen);
    
    while((leo = cola_desencolar(prueba2)) != NULL){
        printf("%s", leo->elemento);
        printf("\n");
    }

    
    
    printf("pastel de papa");
    printf("pastel de papaAAAAA");

    return 0;
}

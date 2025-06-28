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
#include <math.h>

//#include "elemento.h"

enum tipo {OPERADOR, NUMERO, FUNCION, PARENTESIS};
typedef enum tipo tipo;

typedef struct {
    char *elemento;
    tipo tipo;
}elemento_t; 

void elemento_destruir(elemento_t*elemento);

void destruir_elemento(void*elemento){
    elemento_destruir((elemento_t *)elemento);
}

void elemento_destruir(elemento_t*elemento){
    free(elemento->elemento);
    free(elemento);
}

void destruir_racional(void *racional){
    racional_destruir((racional_t *)racional);
}



operador_t *buscar(operador_t **operadores, size_t oplen , char * x) { // buscamos los operadores por "+", "-", ("identificadores"(?))
	for (size_t i = 0; i<oplen; i++){
        if(strcmp(operadores[i]->operador, x) == 0){
            return operadores[i];
        }
    }
    return NULL;
} // si devuelve null no existe el operador


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
//esto esta perfecto


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
    
    while ((element = cola_desencolar(infija)) != NULL){//ok
        if(element->tipo == NUMERO){//ok
            cola_encolar(salida, element);
            continue;
        }
        if(element->tipo == FUNCION || (element->tipo == PARENTESIS && strcmp(element->elemento, "(") == 0)){//ok
            pila_apilar(auxiliar, element);
            continue;
        }
        if (element->tipo == OPERADOR){//element seguro distinto de NULL
            tope = pila_ver_tope(auxiliar);
            if (tope == NULL || strcmp(tope->elemento, "(") == 0){//tope tambien distinto de NULL
                pila_apilar(auxiliar, element);
                continue;
            }
            
            operador_t *tope_aux = buscar(operadores, oplen, tope->elemento);
            if(tope_aux == NULL){
                //liberar memoria
                //printf("el operador %s no existe", tope->elemento);
                return NULL;
            }
            
            operador_t *element_aux = buscar(operadores, oplen, element->elemento);
            if(tope_aux == NULL){
                //liberar memoria
                //printf("el operador %s no existe", (element->elemento));
                return NULL;
            }
            
            while((tope = pila_ver_tope(auxiliar))!= NULL && strcmp(tope->elemento, "(") != 0 && tope_aux -> prioridad >= element_aux -> prioridad){
                tope = pila_desapilar(auxiliar);
                cola_encolar(salida ,tope);
                tope_aux = buscar(operadores, oplen, tope->elemento);
                if(tope_aux == NULL){
                    //liberar memoria
                    //printf("el operador %s no existe", (tope->elemento)); comente los printf de los errores porque me tiran erro (no se porque)
                    return NULL;
                }
            }
            pila_apilar(auxiliar, element);
            continue;
        }
        if(element->tipo == PARENTESIS && strcmp(element->elemento, ")") == 0){ 
            tope = pila_desapilar(auxiliar);
            if(tope == NULL){
                printf("escribiste mal flaco lo primero");
                continue;
            }
            while(tope != NULL && strcmp(tope->elemento, "(") != 0){// aca falta algo mepa, si despues de cerrar hay una funcion tambien se encola
                cola_encolar(salida ,tope);
                tope = pila_desapilar(auxiliar);
            }
            if(tope == NULL){//o es NULL o es "("
                printf("escribiste mal flaco");
                // liberar memoria
                return NULL;
            }
            if((tope = pila_ver_tope(auxiliar)) != NULL  && tope->tipo == FUNCION){
                cola_encolar(salida, pila_desapilar(auxiliar));
                continue;
            }
            continue;    
        }
    }

    tope = pila_ver_tope(auxiliar);
    while((tope = pila_desapilar(auxiliar))!= NULL){
        if(strcmp(tope->elemento,"(")== 0){
            printf("error, no se cerro nunca este parentesis");
            //liberar memoria
            return NULL;
        }
        cola_encolar(salida ,tope);
    }
    //aca capaz hay que destruir un par de cosillas
    // no lo se la verdad.
    
    return salida;

}

racional_t *cadena_a_racional(char *numero);

racional_t *operar_postfija(cola_t *polaca, operador_t **operadores, size_t oplen){ //llegamos aca con la notacion bien escrita (cualquier error se detecta cuando paso de infija a postfija)

    pila_t *pila = pila_crear(); //va a ser una pila con racionales_t
    if (pila == NULL){
        //libero la mem de polaca? (xq si el algoritmo funciona bien, terminas liberandola toda)
        cola_destruir(polaca,destruir_elemento);
        return NULL;
    }
    while (!cola_esta_vacia(polaca)){
        elemento_t *simbolo = cola_desencolar(polaca);

        if (simbolo->tipo == NUMERO){
            racional_t *numero = cadena_a_racional(simbolo->elemento);//falta implementar una funcion q pase de cadena a racional_t (o ver como hacerlo con las funciones q ya tenemos)
            if (!pila_apilar(pila, numero)){
                pila_destruir(pila, destruir_racional);
                cola_destruir(polaca,destruir_elemento);
                return NULL;
            }
        }

        else if (simbolo->tipo == OPERADOR || simbolo->tipo == FUNCION){
            operador_t *operador = buscar(operadores, oplen, simbolo->elemento);
            if (operador == NULL){
                pila_destruir(pila, destruir_racional);
                cola_destruir(polaca, destruir_elemento);
                return NULL;
            }
            int aridad = operador->aridad;
            racional_t *(*funcion) (const racional_t *a, const racional_t *b) = operador->funcion;

            racional_t *a = NULL;
            racional_t *b = NULL;
            
            if (aridad == 1){
                racional_t *a = pila_desapilar(pila);
                if (a == NULL){
                    pila_destruir(pila, destruir_racional);
                    cola_destruir(polaca,destruir_elemento);
                    return NULL;
                }
            }
            if (aridad == 2){
                //el primero q sale es el segundo parametro
                racional_t *b = pila_desapilar(pila);
                racional_imprimir(b);
                if (b == NULL){
                    pila_destruir(pila, destruir_racional);
                    cola_destruir(polaca,destruir_elemento);
                    return NULL;
                }
                racional_t *a = pila_desapilar(pila);
                racional_imprimir(a);
                if (a == NULL){
                    pila_destruir(pila, destruir_racional);
                    cola_destruir(polaca,destruir_elemento);
                    racional_destruir(a);
                    return NULL;
                }
            }

            racional_t *resultado = funcion(a,b);
            if (a != NULL) racional_destruir(a);
            if (b != NULL) racional_destruir(b);
            if (resultado == NULL){
                    cola_destruir(polaca,destruir_elemento);
                    pila_destruir(pila,destruir_racional);
                    return NULL;
            }
            pila_apilar(pila, resultado);
        }
    }
    //deberia quedar un solo elemento pero chequeamos por si las moscas ahre
    racional_t * resultado_final = pila_desapilar(pila);
    if (resultado_final == NULL || !pila_esta_vacia(pila)){
        pila_destruir(pila, destruir_racional);
        if (resultado_final != NULL) racional_destruir(resultado_final);
        cola_destruir(polaca,destruir_elemento);  
        return NULL;     
    }
    pila_destruir(pila,destruir_racional);
    return resultado_final;
}           


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
    tabla[0] = operador_crear("+", racional_sumar, 2, 1, "es una suma");
    tabla[1] = operador_crear("-", racional_restar, 2, 1, "es una resta");
    tabla[2] = operador_crear("*", racional_multiplicar, 2, 2, "es un producto");
    tabla[3] = operador_crear("/", racional_dividir, 2, 3, "es una division");
    //tabla[4] = operador_crear("potencia", racional_elevar, 2, 2, "es una potencia"); //pero hay que hacerla jj
    *n = 4;
    return tabla;
}


racional_t *cadena_a_racional(char *numero){ //anda perfectoooooooo
    //voy a contar el num de digitos dsps de la coma (primero busco el punto y hasta q sea \0 cuento)
    //el racional es: numerador = numero pero saco el punto, denominador = 10^(cant nums dsps de punto)
    
    size_t largo = strlen(numero) ;
    char *num = malloc (largo);
    if (num == NULL) return NULL;
    size_t n_num = 0;
    
    
    for (size_t i=0; i<largo; i++){
        if(numero[i] != '.'){
            num[n_num++] = numero[i] - '0';// ERA MENOS EL 0 ASDASDASDHASHUIFBGSHDB
        }// Y CON EL ARREGLO DADO VUELTA, chau me mato
    }//el formato bcd ese que nos habia dicho santisi era eso, dado vuelta. yo tampoco me acordaba la verdad.

    for(size_t j = 0; j < n_num / 2; j++) {
        char aux = num[j];
        num[j] = num[n_num - j - 1];
        num[n_num - j - 1] = aux;
    }
    
    entero_t *numerador = entero_desde_bcd(num, n_num);
    free (num); //no estoy segura si se libera el arreglo q uso para construir el entero_t
    // no entiendo si no estas seguro si se libera, o si se deberia (pero si se libera).

    size_t punto;
    for (punto = 0; numero[punto] != '\0' && numero[punto] != '.'; punto++);//aca contas los numeros adelante del punto? deberian sar los de atras(?)
    if (punto == largo){
        //la cadena no tiene punto: numerador = numero, denominador = 1
        entero_t *denominador = entero_uno();
        racional_t *racional = racional_crear(false, numerador, denominador);
        return racional;
    }
    //ahora si hay un punto:
    
    char *den = malloc(n_num - punto);

    if (den == NULL){
        entero_destruir(numerador);
        return NULL;
    }

    den[n_num - punto - 1] = 1;
    for(size_t i = 0; i < n_num - punto - 1 ; i++){
        den[n_num - punto - 2 - i] = 0;
    }

    entero_t *denominador = entero_desde_bcd(den, (n_num - punto)); //creo que estos tamanios de char *den son los que estan bien.
    free (den);
    racional_t *racional = racional_crear(false, numerador, denominador);
    return racional;
}

size_t binario_a_bcd(char bcd[], unsigned long entero){
    size_t i=0;
        while (entero > 0){
            bcd[i] = entero % 10;
            i++;
            entero /= 10;
        }

    return i;
}

char *racional_a_cadena(const racional_t *numero, char* acc){//agarro el 10 lo multiplico por precision
    char *dz = {0,1};

    //creo que asi va mejor
    size_t acclen= strlen(acc);
    for(size_t j = 0; j < acclen / 2; j++) {
        char aux = acc[j];
        acc[j] = acc[acclen - j - 1];
        acc[acclen - j - 1] = aux;
    }
    entero_t *precision = entero_desde_bcd(acc, strlen(acc));
    if(precision == NULL) return NULL;
    entero_t *diez = entero_desde_bcd(dz,2);
    if(diez == NULL){
        entero_destruir(precision);
        return NULL;
    }
    if(!entero_multiplicar(diez, precision)){
        entero_destruir(precision);
        entero_destruir(diez);
        return NULL;
    }
    if(!entero_multiplicar(diez, racional_numerador(numero))){
        entero_destruir(precision);
        entero_destruir(diez);
        return NULL;
    }
    entero_t * den = entero_clonar(racional_denominador(numero));
    if(den == NULL){
        entero_destruir(diez);
        entero_destruir(precision);
    }

    if(!entero_dividir(diez, den, NULL)){
        entero_destruir(diez);
        entero_destruir(den);
        entero_destruir(precision);
        return NULL;
    }// ahora en num tengo el numero pero multiplicado diez * precision veces
    
    size_t n;
    char *dev = entero_a_bcd(diez, &n); // en n queda el largo del arreglo, hubiese estado bueno documentar eso xd
    // deberia tener un numero que sea n como entero_t ==> paso el n a char* y hago el entero (tp1??)
    // no hay otra forma???
    char *largo = malloc(log(n) + 2);
    size_t nlen = binario_a_bcd(largo, n);
    entero_t *entero_largo = entero_desde_bcd(largo, nlen);
    entero_t *cero = entero_cero();
    entero_t *uno = entero_uno();
    size_t i = 0;
    size_t j = 0;
    char *nuevo = NULL;
    char *aux = NULL;
    while(entero_comparar(entero_largo,cero) == 0){//aca me fijo que sea distinto de 0???
        if(entero_comparar(entero_largo,precision) == 0){
            //poner el punto
            aux = realloc(nuevo, i + 1);
            if(aux == NULL){
                //liberar memoria
                return NULL;
            }
            nuevo = aux;
            nuevo [i]=  '.';
        }
        entero_restar(entero_largo, uno);
        
        nuevo[i] = dev[j];
        aux = realloc(nuevo, i + 1);
        if(aux == NULL){
            //liberar memoria
            return NULL;
        }
        nuevo = aux;
        entero_restar(entero_largo, uno);
    }//asi deberia andar creo
    
    //casos:
    //    - n es mas chico que acc -> agrego 0s hasta que el largo de dev sea igual a acc, despues agrego el 0 adelante
    //    - n es mas grande que acc -> agrego un punto en acc posiciones desde el final
    //    - n es igual -> agrego un cero adelante
    
    // itero con entero_t. 
    

    return;
}
*/




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
    //size_t oplen;
    //operador_t **operadores = tabla_crear(&oplen);
    cola_t *prueba;
    //cola_t *prueba2;
    
    prueba = leer_linea();
    elemento_t *leo;

    //prueba2 = pasar_a_postfija(prueba, operadores, oplen);
    
    while((leo = cola_desencolar(prueba)) != NULL){
        racional_imprimir(cadena_a_racional(leo->elemento));
        printf("\n");
    }

    //racional_imprimir(operar_postfija(prueba2, operadores, oplen));
    
    printf("pastel de papa");
    printf("pastel de papaAAAAA");

    return 0;
}

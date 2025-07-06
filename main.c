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

void elemento_destruir(elemento_t *elemento){
    free(elemento->elemento);
    free(elemento);
}
void destruir_racional(void *racional){
    racional_destruir((racional_t *)racional);
}


//cosas importantes del trabajo

operador_t *buscar(operador_t **operadores, size_t oplen , char * x) { // buscamos los operadores por "+", "-", ("identificadores"(?))
    
    for (size_t i = 0; i < oplen; i++){
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
    if (c== EOF) {
        cola_destruir(cola,NULL);
        return NULL;
    }
    while(c != EOF && c != '\n') {
        
        if (c == ' '){
            c = getchar();
            continue;
        }
        if (c == '.'){
            return NULL;
        }
        if (isdigit(c)){
            int j = 1;
            char *num = malloc(sizeof(char) * 2);
            if(num == NULL){
                cola_destruir(cola, destruir_elemento);
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
                    cola_destruir(cola,destruir_elemento);
                    return NULL;
                }
                num = aux;
                num[j++] = c;
                c = getchar();
            }
            num[j] = '\0';
            if(n_puntos == 1 &&  num[j-1] == '.'){
                return NULL;
            }
            if(n_puntos == 2){
                return NULL;
            }
            elemento_t *elemento = malloc(sizeof(elemento_t));
            if (elemento == NULL){
                free(num);
                cola_destruir(cola, destruir_elemento);
                return NULL;
            }
            elemento->elemento = num;
            elemento->tipo = NUMERO;

            if (!cola_encolar(cola,elemento)){
                free(elemento->elemento);
                free (elemento);
                cola_destruir(cola,destruir_elemento);
                return NULL;
            }

            continue;
        }
        else if (isalpha(c)){
            int j = 1;
            char *fun = malloc(sizeof(char) * 2); 
            if(fun == NULL){
                cola_destruir(cola, destruir_elemento);
                return NULL;
            }
            fun[0] = c;
            c = getchar();
            while (isalpha(c)){
                char *aux = realloc(fun, sizeof(char) * (j + 2));
                if(aux == NULL) {
                    free(fun);
                    cola_destruir(cola, destruir_elemento);
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
                cola_destruir(cola, destruir_elemento);
                return NULL;
            }
            elemento->elemento = fun;
            elemento->tipo = FUNCION;
            if (!cola_encolar(cola, elemento)){
                free(elemento->elemento); 
                free(elemento);
                cola_destruir(cola, destruir_elemento);
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
                cola_destruir(cola, destruir_elemento);
                return NULL;
            }
            elemento->elemento = operador;
            elemento->tipo = PARENTESIS;
            if (!cola_encolar(cola, elemento)){
                free(elemento->elemento); 
                free(elemento);
                cola_destruir(cola, destruir_elemento);
                return NULL;
            }
            continue;
        }
        else if (strchr("+-*/^_", c) != NULL && c != '\0'){ // esta funcion que encontraste devuelve un puntero a la primera ocurrencia de c en la linea esa, por eso dintinto de null tambien hay que chequear que sea distinto del '\0' era el comentario mas largo de la historia
            char *operador = malloc(sizeof(char) * 2); 
            if(operador == NULL){
                cola_destruir(cola, destruir_elemento);
                return NULL;
            }
            operador[0] = c;
            operador[1] = '\0';
            c =getchar();

            elemento_t *elemento = malloc(sizeof(elemento_t));
            if (elemento == NULL){
                free(operador);
                cola_destruir(cola, destruir_elemento);
                return NULL;
            }
            elemento->elemento = operador;
            elemento->tipo = OPERADOR;
            if (!cola_encolar(cola, elemento)){
                free(elemento->elemento); 
                free(elemento);
                cola_destruir(cola, destruir_elemento);
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
            //racional_imprimir(numero);
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
            //printf("%d", aridad);
            racional_t *(*funcion) (const racional_t *a, const racional_t *b) = operador->funcion;
            racional_t *a = NULL;
            racional_t *b = NULL;
            
            if (aridad == 1){
                a = pila_desapilar(pila);
                if (a == NULL){
                    pila_destruir(pila, destruir_racional);
                    cola_destruir(polaca,destruir_elemento);
                    return NULL;
                }
            }
            if (aridad == 2){
                //el primero q sale es el segundo parametro
                b = pila_desapilar(pila);
                //racional_imprimir(b);
                if (b == NULL){
                    pila_destruir(pila, destruir_racional);
                    cola_destruir(polaca,destruir_elemento);
                    return NULL;
                }
                /*racional_t * esta definido arriba*/a = pila_desapilar(pila);
                //racional_imprimir(a);
                if (a == NULL){
                    pila_destruir(pila, destruir_racional);
                    cola_destruir(polaca,destruir_elemento);
                    racional_destruir(b);
                    return NULL;
                }
            }
            //racional_imprimir(a);
            //racional_imprimir(b);
            racional_t *resultado = funcion(a,b);
            racional_destruir(a);
            racional_destruir(b);
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
    //racional_imprimir(resultado_final);

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
int prioridad, char *descripcion, size_t *n){
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
    *n= *n + 1;
    return nuevo;
}




//ENVOLTORIOS

racional_t *inverso_racional (const racional_t *a, const racional_t *b){
    return racional_inverso(a);
}
racional_t *factorial_racional (const racional_t *a, const racional_t *b){
    return racional_factorial(a);
}
racional_t *abs_racional (const racional_t *a, const racional_t *b){
    return racional_abs(a);
}
racional_t *inverso_multiplicativo_racional(const racional_t *a, const racional_t *b){
    return racional_inverso_multiplicativo(a);
}
racional_t *pi_racional(const racional_t *a, const racional_t *b){
    return racional_pi();
}
racional_t *e_racional(const racional_t *a, const racional_t *b){
    return racional_e();
}
racional_t *phi_racional(const racional_t *a, const racional_t *b){
    return racional_phi();
}

operador_t **tabla_crear(size_t *n){
    operador_t **tabla = malloc(20 * sizeof(operador_t*));// si fueran mas de 20 agregar aca primero 
    if(tabla == NULL){
        return NULL;
    }
    tabla[0] = operador_crear("+", racional_sumar, 2, 1, "es una suma", n);
    tabla[1] = operador_crear("-", racional_restar, 2, 1, "es una resta", n);
    tabla[2] = operador_crear("*", racional_multiplicar, 2, 2, "es un producto", n);
    tabla[3] = operador_crear("/", racional_dividir, 2, 3, "es una division", n);
    tabla[4] = operador_crear("_", inverso_racional, 1, 5, "(-1)", n);
    tabla[5] = operador_crear("fact", factorial_racional, 1, 9, "factorial", n);
    tabla[6] = operador_crear("abs", abs_racional, 1, 9, "modulo", n);
    tabla[7] = operador_crear("inv", inverso_multiplicativo_racional, 1, 9, "inverso multiplicativo(?", n);
    tabla[8] = operador_crear("pi", pi_racional, 0, 9, "pi", n);
    tabla[9] = operador_crear("e", e_racional, 0, 9, "eeeee", n);
    tabla[10] = operador_crear("phi", phi_racional, 0, 9, "fibo", n);
    tabla[11] = operador_crear("^", racional_elevar, 2, 2, "es una potencia",n); //pero hay que hacerla jj
    return tabla;
}//asi esta bien

racional_t *cadena_a_racional(char *numero){ //anda perfectoooooooo
    //voy a contar el num de digitos dsps de la coma (primero busco el punto y hasta q sea \0 cuento)
    //el racional es: numerador = numero pero saco el punto, denominador = 10^(cant nums dsps de punto)
    if (numero == NULL) return NULL;
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
        entero_destruir(numerador);
        entero_destruir(denominador);
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
    entero_destruir(denominador);
    entero_destruir(numerador);
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

char *racional_a_cadena(const racional_t *numero, char* acc){//agarro el 10 lo elevo precision
    
    if (numero == NULL){
        return NULL;
    }
    entero_t *cero = entero_cero();
    if(entero_comparar(racional_numerador(numero), cero) == 0){
        printf("0");
        entero_destruir(cero);
        return NULL;
    }
    entero_t *uno = entero_uno();
    //int es_uno = entero_comparar(racional_denominador(numero), uno);
    char dz[] = {0,1};

    size_t acclen= strlen(acc);
    char *bcd = malloc(acclen);
    if (bcd == NULL) return NULL;
    for(size_t i = 0; i < acclen ; i++){
        bcd[i] = acc[acclen - i - 1] - '0';
    }
    entero_t *precision = entero_desde_bcd(bcd, acclen);
    entero_imprimir(precision);
    /*if(es_uno != 0) {
        if(!entero_restar(precision, uno)){

        }
    }*/
    printf("\n");
    free(bcd);
    if(precision == NULL){
        entero_destruir(uno);
        entero_destruir(cero);

        return NULL;
    }


    entero_t *diez = entero_desde_bcd(dz,2);
    if(diez == NULL){
        entero_destruir(precision);
        entero_destruir(cero);
        entero_destruir(uno);
        return NULL;
    }
    if(!entero_elevar(diez, precision)){// aca no es multiplicar, hay que elevar a precision. sigo cuando tenga esta de elevar.
        entero_destruir(cero);
        entero_destruir(precision);
        entero_destruir(uno);
        return NULL;
    }
    if(!entero_multiplicar(diez, racional_numerador(numero))){
        entero_destruir(cero);
        entero_destruir(precision);
        entero_destruir(diez);
        entero_destruir(uno);
        return NULL;
    }
    if(!entero_dividir(diez, racional_denominador(numero), NULL)){
        entero_destruir(diez);
        entero_destruir(precision);
        entero_destruir(cero);
        entero_destruir(uno);
        return NULL;
    }
    entero_imprimir(diez);// hasta aca estamos.
   
    char *aux = NULL;
    size_t n = 0;
    char *dev1 = entero_a_bcd(diez, &n); // en n queda el largo del arreglo, hubiese estado bueno documentar eso xd
    char *dev = malloc(n + 1);
    printf("\nn dev es:%lu", n);

    for(size_t i = 0; i < n; i++){
        dev [i] = dev1 [n - 1 - i] + '0';
    }
    dev[n] = '\0';
    
    free(dev1);

    char *largo = malloc(log(n) + 2);
    size_t nlen = binario_a_bcd(largo, n);
    entero_t *entero_largo = entero_desde_bcd(largo, nlen);
    if(entero_largo == NULL){
        entero_destruir(uno);
        entero_destruir(cero);
        entero_destruir(diez);
        entero_destruir(precision);
        free(largo);
    }
    
    size_t i = 0;
    size_t j = 0;
    char *nuevo = NULL;
    printf("\n largo es: ");
    entero_imprimir(entero_largo);// entero largo seria la cantidad de digitos que tiene el numero que voy a devolver
    printf("\n precision es: ");
    /*if (es_uno != 0) {
        if(!entero_sumar(precision, uno)){

        }
    }*/
    entero_imprimir(precision);// precision seria acc???
    printf("\n");
    printf("dev es = %s", dev);
    bool in = false;
    while(entero_comparar(entero_largo,cero) != 0){//aca me fijo que sea distinto de 0??? deberia abarcar todos los casos
        
        if(in == false && entero_comparar(entero_largo,precision) < 0){
            aux = realloc(nuevo, i + 2);
            if(aux == NULL){
                entero_destruir(uno);
                entero_destruir(diez);
                entero_destruir(precision);
                entero_destruir(cero);
                entero_destruir(entero_largo);
                free(largo);

                return NULL;
            }
            nuevo = aux;
            nuevo [0] =  '0';
            nuevo [1] = '.';
            i += 2;
            if(!entero_restar(precision, uno)){
                //liberar memoria y hacer algo al respecto
            }
            while (entero_comparar(entero_largo,precision) != 0){
                aux = realloc(nuevo, i + 2);
                if(aux == NULL){
                entero_destruir(uno);
                entero_destruir(diez);
                entero_destruir(precision);
                entero_destruir(cero);
                entero_destruir(entero_largo);
                free(largo);

                return NULL;
            }
                nuevo = aux;
                nuevo[i] = '0';
                i++;
                if(!entero_restar(precision, uno)){
                    entero_destruir(cero);
                    entero_destruir(diez);
                    entero_destruir(precision);
                    entero_destruir(uno);
                    entero_destruir(entero_largo);
                    free(nuevo);
                    free(largo);

                    return NULL;
                }   
            }
            if(!entero_sumar(precision,uno)){
                //aca lo mismo
            }
            while(entero_comparar(precision,cero) != 0){
                aux = realloc(nuevo, i + 2);
                if(aux == NULL){
                    entero_destruir(cero);
                    entero_destruir(diez);
                    entero_destruir(precision);
                    entero_destruir(uno);
                    entero_destruir(entero_largo);
                    free(largo);
                
                    return NULL;
                }
                nuevo = aux;
                nuevo[i] = dev[j];
                i++;
                j++;
                if(!entero_restar(precision, uno)){
                    entero_destruir(cero);
                    entero_destruir(diez);
                    entero_destruir(precision);
                    entero_destruir(uno);
                    entero_destruir(entero_largo);
                    free(nuevo);
                    free(largo);
                
                    return NULL;
                }
            }
            break;
        }


        if(entero_comparar(entero_largo,precision) ==  0){
            aux = realloc(nuevo, i + 2);
            if(aux == NULL){
                entero_destruir(uno);
                entero_destruir(diez);
                entero_destruir(precision);
                entero_destruir(cero);
                entero_destruir(entero_largo);
                free(largo);

                return NULL;
            }
            nuevo = aux;
            nuevo [i]=  '.';
            i++;
            in = true;
        }
        
        aux = realloc(nuevo, i + 2);
        if(aux == NULL){
            entero_destruir(cero);
            entero_destruir(diez);
            entero_destruir(precision);
            entero_destruir(uno);
            entero_destruir(entero_largo);
            free(largo);

            return NULL;
        }
        nuevo = aux;
        nuevo[i] = dev[j];
        i++;
        j++;
        if(!entero_restar(entero_largo, uno)){
            entero_destruir(cero);
            entero_destruir(diez);
            entero_destruir(precision);
            entero_destruir(uno);
            entero_destruir(entero_largo);
            free(nuevo);
            free(largo);
            return NULL;
        }
    }
    entero_destruir(cero);
    entero_destruir(diez);
    entero_destruir(precision);
    entero_destruir(uno);
    entero_destruir(entero_largo);
    free(largo);
    nuevo[i]= '\0';
    return nuevo;
}

int main(int argc, char *argv[]){
    if (argc > 2) return 1;
    size_t oplen = 0;
    if (argc == 1){
        printf("escribir %s ayuda",argv[0]);
        return 0;
    }
    operador_t **operadores = tabla_crear(&oplen);
    if (strcmp(argv[1],"ayuda") == 0){
        printf("Calculadora TA130\n");
        printf("Uso: %s [OPCIONES]\n", argv[0]);
        printf("OPCIONES:\n");
        printf("    ayuda: Muestra esta ayuda.\n");
        printf("    racional: Selecciona salida racional.\n");
        printf("    <n>: Salida con <n> dígitos de precisión.\n");
        printf("OPERADORES:\n");
        for(size_t i=0; i<oplen; i++){
            printf("    ");
            if(operadores[i]->aridad == 2){
                printf("a%sb: %s\n",operadores[i]->operador,operadores[i]->descripcion);
            }
            if(operadores[i]->aridad == 1){
                printf("%s(a): %s\n", operadores[i]->operador, operadores[i]->descripcion);
            }
            if(operadores[i]->aridad == 0){
                printf("%s: %s\n", operadores[i]->operador, operadores[i]->descripcion);
            }
        }
        //tabla_destruir
        return 0;
    }

    while(1){
        cola_t *input_infija = leer_linea();
        if (input_infija == NULL){
            //hay que hacer un tabla_destruir?
            printf("error fatal. batata");
            return 1;
        }
        cola_t *input_postfija = pasar_a_postfija(input_infija, operadores, oplen);
        cola_destruir(input_infija, NULL);
        if (input_postfija == NULL){
            //tabla_destruir
            printf("error fatal. espinaca");
            return 1;
        }
        racional_t *resultado = operar_postfija(input_postfija, operadores, oplen);
        if (resultado == NULL){
            printf("error fatal. tomate");
            cola_destruir(input_postfija, destruir_elemento);
            //tabla_destruir
            return 1;
        }
        if (argc == 1){
            //si no aclaran nada se imprime con 8 digitos de precision
            char *rta = racional_a_cadena(resultado, "8");
            racional_destruir(resultado);
            if (rta == NULL){
                printf("error fatal. espinaca");
                //tabla_destruir
                return 1;
            }
            printf("%s\n",rta);
            free(rta);
        }
        else if(strcmp(argv[1],"racional") == 0){
            if (!racional_imprimir(resultado)){
                racional_destruir(resultado);
                //tabla_destruir
                printf("error fatal. espinaca");

                return 1;
            }
            printf("\n");
            racional_destruir(resultado);
        }
        else{
            char *rta = racional_a_cadena(resultado, argv[1]);
            racional_destruir(resultado);
            if (rta == NULL){
                //tabla_destruir
                return 1;
                printf("error fatal. espinaca");

            }
            printf("la respuesta es: %s\n",rta);
            free(rta);
        }
    }

    //tabla_destruir
    return 0;
}


/*
struct racional {
    bool s;
    entero_t *n, *d;
};

int main(){    
    //size_t oplen = 0;
    //operador_t **operadores = tabla_crear(&oplen);
    cola_t *prueba;
    //cola_t *prueba2;
    //cola_t *prueba3;
    
    prueba = leer_linea();
    elemento_t *leo;
    //prueba2 = pasar_a_postfija(prueba, operadores, oplen);
    //prueba3 = pasar_a_postfija(prueba, operadores, oplen);
    //char acc[]= "50";
    leo = cola_desencolar(prueba);
    racional_t *b = cadena_a_racional(leo->elemento);
    elemento_destruir(leo);
    leo = cola_desencolar(prueba);
    racional_t *e = cadena_a_racional(leo->elemento);   
    elemento_destruir(leo);
    entero_multiplicar(b->n, e->n);
    entero_imprimir(b->n);
    //while((leo = cola_desencolar(prueba)) != NULL){
    //    cadena_a_racional(leo->elemento);
    //    
    //    printf("\n");
    //}
    //printf("\n");
    //char *res = racional_a_cadena(operar_postfija(prueba2, operadores, oplen), acc);
    //printf("%s",res);
    //printf("\n");
    racional_destruir(e);
    racional_destruir(b);
    printf("\n");
    printf("pastel de papa");
    printf("pastel de papaAAAAA");
    cola_destruir(prueba, destruir_elemento);
    return 0;
}
*/
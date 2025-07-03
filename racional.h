#ifndef RACIONAL_H
#define RACIONAL_H

#include "entero.h"
#include <stdbool.h>




//tipo
typedef struct racional racional_t;

//constructores
racional_t *racional_crear(bool es_negativo, entero_t *numerador, entero_t *denominador);//creo el racional ya reducido

//destructores
void racional_destruir(racional_t *q);

//getters
const entero_t *racional_numerador(const racional_t *q);
const entero_t *racional_denominador(const racional_t *q);
bool racional_es_negativo(const racional_t *q);
bool racional_imprimir(const racional_t *q);

//primitivas
entero_t *maximo_comun_divisor(const entero_t *n,const entero_t *d);
racional_t *racional_sumar(const racional_t *q, const racional_t *r);
racional_t *racional_restar(const racional_t *q, const racional_t *r);
racional_t *racional_multiplicar(const racional_t *q, const racional_t *r);
racional_t *racional_dividir(const racional_t *q, const racional_t *r);
racional_t *racional_factorial(const racional_t *n);
racional_t *racional_abs(const racional_t *q);
racional_t *racional_inverso(const racional_t *q);
racional_t *racional_inverso_multiplicativo(const racional_t *q);
racional_t *racional_pi ();
racional_t *racional_e ();
racional_t *racional_phi ();
racional_t *racional_elevar(const racional_t *b,const racional_t *e);


#endif
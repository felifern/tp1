#ifndef ENTERO_H
#define ENTERO_H

#include<stdbool.h>
#include<stdio.h>
//tipo
typedef struct entero entero_t;

//constructores
entero_t *entero_desde_bcd(const char bcd[], size_t n);//
entero_t *entero_cero();//
entero_t *entero_uno();//
entero_t *entero_clonar(const entero_t *entero);//


//destructores
void entero_destruir(entero_t *entero);//

//getters
bool entero_imprimir(const entero_t *entero);//

//primitivas
int entero_comparar(const entero_t *a, const entero_t *b);//
bool entero_desplazar_derecha(entero_t *e);//
bool entero_desplazar_izquierda(entero_t *e);//
bool entero_sumar(entero_t *a, const entero_t *b);//
bool entero_factorial(entero_t *n);
bool entero_restar(entero_t *a, const entero_t *b);//
bool entero_multiplicar(entero_t *a, const entero_t *b);//
bool entero_dividir(entero_t *a, const entero_t *b, entero_t **resto);//
char *entero_a_bcd(const entero_t *entero, size_t *n);//


#endif
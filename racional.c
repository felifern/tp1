#include <stdbool.h>
#include "entero.h"
#include <stdlib.h>
#include <string.h>

struct racional {
    // Representa una fracción mixta de la forma (-1)^s * (n / d).
    // s es el signo, n el numerador y d el denominador.
    // Invariante de representación:
    // *) n y d diferentes de NULL.
    // *) s será false o true según el número sea positivo o negativo respetivamente.
    // *) La fracción es irreducible, es decir n y d son coprimos.
    // *) d >= 1 (si n != 0).
    // *) Si n == 0 => d == 1 && s == false.
    bool s;
    entero_t *n, *d;
};

typedef struct racional racional_t;


bool racional_imprimir(const racional_t *q);

entero_t *maximo_comun_divisor(const entero_t *n,const entero_t *d){
    if(n == NULL || d == NULL) return NULL;
    entero_t *uno = entero_uno();
    entero_t *cero = entero_cero();
    if(entero_comparar(n,cero) == 0){
        entero_destruir(cero);
        entero_destruir(uno);
        return entero_clonar(d);
    }

    if(entero_comparar(n,d) == 0){ 
        entero_destruir(cero);
        entero_destruir(uno);
        return entero_clonar(n);
    }
    if(entero_comparar(uno,d)== 0){
        entero_destruir(cero);
        return uno;
    }


    entero_t *nc = entero_clonar(n);
    entero_t *dc = entero_clonar(d);
    entero_t *r;
    
    
    if(entero_comparar(nc,dc)>0){
        if(!entero_dividir(nc,dc,&r)){
            entero_destruir(nc);
            entero_destruir(dc);
            entero_destruir(r);
            entero_destruir(cero);
            entero_destruir(uno);
            return NULL;
        }
        while(entero_comparar(r,cero) != 0){
            entero_destruir(nc);
            nc = entero_clonar(dc);
            entero_destruir(dc);
            dc = entero_clonar(r);
            entero_destruir(r);
            
            if(!entero_dividir(nc,dc,&r)){
                entero_destruir(nc);
                entero_destruir(dc);
                entero_destruir(r);
                entero_destruir(cero);
                entero_destruir(uno);
                return NULL;
            }
        }
        entero_destruir(cero);
        entero_destruir(uno);
        entero_destruir(nc);
        entero_destruir(r);
        return dc;
    }
    if(entero_comparar(nc,dc)<0){
        if(!entero_dividir(dc,nc,&r)){
            entero_destruir(dc);
            entero_destruir(nc);
            entero_destruir(r);
            entero_destruir(cero);
            entero_destruir(uno);

            return NULL;
        }
        while(entero_comparar(r,cero) != 0){
            entero_destruir(dc);
            dc = entero_clonar(nc);
            entero_destruir(nc);
            nc = entero_clonar(r);
            entero_destruir(r);

            if(!entero_dividir(dc,nc,&r)){
                entero_destruir(dc);
                entero_destruir(nc);
                entero_destruir(r);
                entero_destruir(cero);
                entero_destruir(uno);
                return NULL;
            }
        }
        entero_destruir(dc);
        entero_destruir(r);
        entero_destruir(cero);
        entero_destruir(uno);

        return nc;
    }
    
    return NULL;
}

bool racional_reducir (racional_t *c){
    entero_t *mcd = maximo_comun_divisor(c->n, c->d);
    if(mcd == NULL) return false;
    entero_t *cero = entero_cero();
    /*
    printf("\n");
    printf("racioanl es: ");
    racional_imprimir(c);
    printf("\n");

    printf("mcd es: ");
    entero_imprimir(mcd);
    printf("\n");
    */
    if((entero_comparar(c->n,cero) == 0)){
        if(!entero_dividir(c->d, mcd, NULL)) return false;
        entero_destruir(mcd);
        entero_destruir(cero);
        return true;
    }
    if(!entero_dividir(c->n, mcd, NULL)){
        entero_destruir(cero);
        entero_destruir(mcd);
        return false;
    }
    if(!entero_dividir(c->d, mcd, NULL)){
        entero_destruir(cero);
        entero_destruir(mcd);
        return false;
    }
    entero_destruir(mcd);
    entero_destruir(cero);
    return true;
} 

racional_t *racional_crear(bool es_negativo, entero_t *numerador, entero_t *denominador){//creo el racional ya reducido
    if(numerador == NULL) return NULL;
    entero_t *cero = entero_cero();
    racional_t *racional = malloc(sizeof(racional_t));

    if(denominador == NULL || entero_comparar(numerador, cero) == 0){
        racional->d = entero_uno();
        racional->n = entero_clonar(numerador);
        if(entero_comparar(numerador, cero) == 0) racional->s = false;
        else racional->s = es_negativo;
    }
    else if(entero_comparar(denominador, cero) == 0){
        entero_destruir(cero);
        free(racional);
        return NULL;
    }
    else{
        racional->d = entero_clonar(denominador);
        racional->n = entero_clonar(numerador);
        racional->s = es_negativo;
    }
    if(!racional_reducir(racional)){
        entero_destruir(racional->d);
        entero_destruir(racional->n);
        free(racional);
        entero_destruir(cero);

        return NULL;
    }   
    entero_destruir(cero);
    return racional;

}

void racional_destruir(racional_t *q){
    if(q == NULL) return;
    if(q->n != NULL) entero_destruir(q->n);
    if(q->d != NULL) entero_destruir(q->d);
    if(q != NULL) free(q);
}

const entero_t *racional_numerador(const racional_t *q){
    if(q == NULL) return false;
    return q->n;
}

const entero_t *racional_denominador(const racional_t *q){
    if(q == NULL) return false;
    return q->d;
}

bool racional_es_negativo(const racional_t *q){
    if(q == NULL) return false;
    return q->s;
}

bool racional_imprimir(const racional_t *q){
    printf("%s", q->s ? "-":"+");
    if(!entero_imprimir(q->n)) return false;
    printf("/");
    if(!entero_imprimir(q->d)) return false;
    return true;
}

racional_t *racional_sumar(const racional_t *q, const racional_t *r){
    racional_t *qc;
    entero_t *aux1 = entero_clonar(q->n);
    entero_t *aux2 = entero_clonar(q->d);

    if ((qc = racional_crear(q->s,aux1, aux2))== NULL){
        entero_destruir(aux1);
        entero_destruir(aux2);
        return NULL;
    }
    entero_destruir(aux1);
    entero_destruir(aux2);
    aux1 = entero_clonar(r->n);
    aux2 = entero_clonar(r->d);

    racional_t *rc;
    if ((rc = racional_crear(r->s,aux1, aux2))== NULL){
        entero_destruir(aux1);
        entero_destruir(aux2);
        racional_destruir(qc);
        return NULL;
    }
    entero_destruir(aux1);
    entero_destruir(aux2);
    if(!entero_multiplicar(qc->n,r->d)){
        racional_destruir(qc);
        racional_destruir(rc);
        return NULL;
    }
    if(!entero_multiplicar(rc->n,q->d)){
        racional_destruir(qc);
        racional_destruir(rc);
        return NULL;
    }
    if(!entero_multiplicar(qc->d,r->d)){
        racional_destruir(qc);
        racional_destruir(rc);
        return NULL;
    }
    if(qc->s == rc->s){
        if(!entero_sumar(qc->n, rc->n)){
            racional_destruir(qc);
            racional_destruir(rc);
            return NULL;
        }
    }
    else if(qc->s != rc->s){
        if(entero_comparar(qc->n,rc->n) > 0){
            if(entero_restar(qc->n, rc->n) == false){
                racional_destruir(qc);
                racional_destruir(rc);
                return NULL;
            }
        }
        else if(entero_comparar(qc->n,rc->n) < 0){
            if(entero_restar(rc->n, qc->n) == false){
                racional_destruir(qc);
                racional_destruir(rc);
                return NULL;
            }
            entero_destruir(qc->n);
            qc->n = entero_clonar(rc->n);
            qc->s = !(qc->s);
        }
        else{
            if(entero_restar(qc->n, rc->n) == false) return NULL;
            qc->s = false;
        }
    }
    if(!racional_reducir(qc)) return NULL;
    racional_destruir(rc);
    return qc;
}

racional_t *racional_restar(const racional_t *q, const racional_t *r){
    racional_t *rc;
    entero_t *aux1 = entero_clonar(r->n);
    entero_t *aux2 = entero_clonar(r->d);
    if ((rc = racional_crear(!r->s,aux1, aux2)) == NULL){
        entero_destruir(aux1);
        entero_destruir(aux2);
        free(rc);
        return NULL;
    }

    entero_destruir(aux1);
    entero_destruir(aux2);

    racional_t *resta = racional_sumar(q,rc);
    if(!racional_reducir(resta)) return NULL;
    racional_destruir(rc);
    return resta;
}

racional_t *racional_multiplicar(const racional_t *q, const racional_t *r){
    racional_t *qc;
    entero_t *aux1 = entero_clonar(q->n);
    entero_t *aux2 = entero_clonar(q->d);
    if ((qc = racional_crear(q->s,aux1, aux2))== NULL){
        entero_destruir(aux1);
        entero_destruir(aux2);
        free(qc);
        return NULL;
    }
    entero_destruir(aux1);
    entero_destruir(aux2);
    
    qc->s = (q->s ^ r->s);
    if(!entero_multiplicar(qc->n,r->n)){
        racional_destruir(qc);
        return NULL;
    }
    if(!entero_multiplicar(qc->d,r->d)){
        racional_destruir(qc);
        return NULL;
    }
    if(!racional_reducir(qc)) return NULL;
    return qc;
}

racional_t *racional_dividir(const racional_t *q, const racional_t *r){
    racional_t *qc;
    entero_t *aux1 = entero_clonar(q->n);
    entero_t *aux2 = entero_clonar(q->d);

    if ((qc = racional_crear(q->s,aux1, aux2))== NULL){
        entero_destruir(aux1);
        entero_destruir(aux2);
        return NULL;
    }
    
    entero_destruir(aux1);
    entero_destruir(aux2);
    
    qc->s = (q->s ^ r->s);
    if(!entero_multiplicar(qc->n,r->d)){
        racional_destruir(qc);
        return NULL;
    }
    if(!entero_multiplicar(qc->d,r->n)){
        racional_destruir(qc);
        return NULL;
    }
    if(!racional_reducir(qc)) return NULL;
    return qc;  
}

racional_t *racional_factorial(const racional_t *q){
    if (entero_comparar(q->d, entero_uno()) != 0 || q->s == true) return NULL;// esto va a terner una leak, porque cada vez que comparo hago un num(estoy probando)
    entero_t *nuevo_n = entero_clonar(q->n);
    if (nuevo_n == NULL) return NULL;

    if (!entero_factorial(nuevo_n)){
        entero_destruir(nuevo_n);
        return NULL;
    } 
    racional_t *nuevo_q = racional_crear(q->s,nuevo_n,q->d);
    if (nuevo_q == NULL){
        entero_destruir(nuevo_n);
        return NULL;
    }
    return nuevo_q;
}

racional_t *racional_abs(const racional_t *q){
    racional_t *q_abs = racional_crear(false ,q->n, q->d);
    if (q_abs == NULL) return NULL;
    return q_abs;
}

racional_t *racional_inverso(const racional_t *q){
    racional_t  *q_inv = racional_crear(!q->s, q->n, q->d);
    if (q_inv == NULL) return NULL;
    return q_inv;
}

racional_t *racional_inverso_multiplicativo(const racional_t *q){
    racional_t *q_inv = racional_crear(q->s,q->d,q->n);
    if (q_inv == NULL) return NULL;
    return q_inv;
}

racional_t *racional_elevar(const racional_t *b,const racional_t *e){
    racional_t *res;
    if(b == NULL) return NULL;
    if(e == NULL) return NULL; // estas dos validaciones no se.

    if(e->s) res = racional_crear(b->s,b->d,b->n);
    else res = racional_crear(b->s,b->n,b->d);
    if(res == NULL) return NULL;
    if(entero_es_par(e->n)) res->s = false;
    if(!entero_elevar(res->n, e->n)){
        racional_destruir(res);
        return NULL;
    }
    if(!entero_elevar(res->d, e->n)){
        racional_destruir(res);
        return NULL;
    }
    return res;
}

racional_t *racional_pi (){
    char n[] = {6,1,4,1,3};
    char d[] = {0,0,0,0,1};
    entero_t *num = entero_desde_bcd(n,5);
    if (num == NULL) return NULL;
    entero_t *den = entero_desde_bcd(d,5);
    if (den == NULL){
        entero_destruir(num);
        return NULL;
    }
    return racional_crear(0,num,den); 
}

racional_t *racional_e (){
    char n[] = {2,8,1,7,2};
    char d[] = {0,0,0,0,1};
    entero_t *num = entero_desde_bcd(n,5);
    if (num == NULL) return NULL;
    entero_t *den = entero_desde_bcd(d,5);
    if (den == NULL){
        entero_destruir(num);
        return NULL;
    }
    return racional_crear(0,num,den); 
}

racional_t *racional_phi (){
    char n[] = {8,1,6,1};
    char d[] = {0,0,0,1};
    entero_t *num = entero_desde_bcd(n,4);
    if (num == NULL) return NULL;
    entero_t *den = entero_desde_bcd(d,4);
    if (den == NULL){
        entero_destruir(num);
        return NULL;
    }
    return racional_crear(0,num,den); 
}


#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define OVERFLOW_MASK  0x100000000
#define OVERFLOW_SHIFT  32
#define PRIMEROS_32_DIGITOS 0xFFFFFFFF
#define LSB 0x1
#define MSB32 0x80000000

/*
    No hay nulos.
    No estan normalizados (sin 0s adelante).

*/

typedef struct {
    uint32_t *d;
    size_t n;
} entero_t;


entero_t entero_factorial(entero_t *n);// hay que implementar. (feli)



entero_t *_crear(size_t k){ // devuelve un entero_t con el los digitos en 0 de cantidad k 
    uint32_t *aux;
    entero_t *entero = malloc(sizeof (entero_t));
    if(entero == NULL){
        return NULL;
    } 
    entero -> n = k;
    aux = malloc(k * sizeof(uint32_t));
    if(aux == NULL){
        free(entero);
        return NULL;
    } 
    for(size_t i = 0; i < k ; i++){
        aux[i] = 0;
    }
    entero -> d = aux;
    return entero;
}

entero_t *entero_cero() {
    entero_t *e = _crear(1);
    e->d[0]= 0;
    return e;    
}

entero_t *entero_uno() {
    entero_t *e = _crear(1);
    e->d[0] = 1;
    return e;
}

void entero_destruir(entero_t *entero){
    free(entero ->d);
    free(entero);
}

entero_t *entero_clonar(const entero_t *entero){
    entero_t *entero2 = _crear(entero->n);
    if(entero2 == NULL)
        return NULL;
    for (size_t i = 0 ;i < (entero2 -> n) ; i++ ){
        entero2->d[i]=entero->d[i];
    }
    entero2->n = entero->n;
    return entero2;
}

bool _redimensionar(entero_t *entero, size_t k){
    if(k == entero ->n){
        return true;
    }
    uint32_t *aux;
    aux = realloc(entero->d, k * sizeof(uint32_t));
    if (aux == NULL){
        return false;
    }
    entero->d = aux;      
    for (size_t i = entero->n; i<k ; i++){
        entero->d[i]=0;
    }
    entero->n = k;
    return true;
    
}

int entero_comparar(const entero_t *a, const entero_t *b){
    if(a == NULL && b == NULL){
        return 0;
    }
    if(a == NULL){
        return -1;
    }    
    if(b == NULL){
        return 1;
    }
    size_t aux = a->n;
    if ((a->n) > (b->n)){
        for (size_t i = 0; i <= (a->n) - (b->n) - 1 ; i++){
            if(a->d[(a->n)- 1 - i] != 0)
                return 1;
        }
        aux = b->n;
    }
    else if((a->n) < (b->n)){
        for (size_t i = 0; i <= (b->n) - (a->n) - 1 ; i++){
            if (b->d[(b->n)- i - 1] != 0)
                return -1; 
        }
        aux = a->n;
    }
    for (size_t i = 0; i<aux ; i++){
        if(a->d[aux - 1 - i] > b->d[aux - 1 - i]){
            return 1;
        }else if(a->d[aux - 1 - i] < b->d[aux - 1 - i]){
            return -1;
        }
    }
    return 0;
}

bool entero_desplazar_derecha(entero_t *e){//ESTO EN BITSSS
    size_t largo = e->n;
    int carry = 0;
    for(size_t i = 0; i<largo ; i++){
        uint32_t aux = e->d[largo - 1 - i];
        
        e->d[largo - 1 - i] >>= 1;
        if (carry == 1 && i != 0){
            (e->d[largo - 1 - i]) |= MSB32;
            carry = 0;
        }
        
        if((aux & LSB) == 1){
            carry = 1;
        }
        
    }
    return true;
}

bool entero_desplazar_izquierda(entero_t *e){// desplaza a izquierda en el sentido de bits bien ordenados
    size_t largo = e->n;
    int carry = 0;
    for(size_t i = 0; i<largo; i++){
        uint32_t aux = e->d [i];
        e->d[i] <<= 1;
        if(carry == 1){
            e->d[i] |= LSB;
            carry = 0;
        }
        if((aux & MSB32) != 0){
            carry = 1;
        }
    }
    if (carry != 0){
        
        if (_redimensionar(e, e->n + 1)== false){
            return false;
        }
        e->d[e->n - 1] |= LSB;
    }
    return true;


}

char *entero_a_bcd(const entero_t *entero, size_t *n){
    entero_t *entero2 = entero_clonar (entero);
    if(entero2 == NULL){
        return NULL;
    }

    size_t iteraciones = entero ->n * 32; //cantidad de bits de mi numero (si tiene menos no deberia afectar)

    if(!_redimensionar(entero2,2 * entero2->n + 4 * (entero2->n/3) )){ //agrego espacio para el bcd (aca habia un problema, no se bien cual era pero me tiraba un error mortal)
        return NULL;
    }

    for (size_t i = 0  ; i < iteraciones; i++ ){
        for (size_t j = entero -> n; j < entero2 -> n; j++ ){
            for (size_t k = 0; k<8; k++){
                uint32_t numero = ((entero2->d [j] & (0xF << 4*k)) >> 4*k);
                if(numero > 4){
                    numero += 3;
                    entero2->d [j] &= (~((0xF) << 4*k));
                    entero2->d [j] |= (numero << 4*k);
                }
            }
        }
        
        if(!entero_desplazar_izquierda(entero2)){
            return NULL;
        }
    }
    size_t contador = 0;
    int control = 0;
    for (size_t j = (entero2->n - 1); j >= entero->n; j--){
        for (size_t k = 0; k<8; k++){
            if ((entero2->d [j] & (0xF << (28 - (4*k)))) >> (28 -4*k) != 0){
                control = 1;
            }
            if (control == 1){
                *n += 1;
            }
            contador ++;
        }
    }

    char *a = malloc(sizeof(char) * contador);// se puede pedir *n memoria pero en la iteracion de abajo deberia cambiar el limite de arriba
    if(a == NULL){
        entero_destruir(entero2);
        return NULL;
    }


    for (size_t j = entero->n; j < entero2->n; j++){// si pido *n memoria deberia iterar hasta entero->n + *n
        for (size_t k = 0; k<8; k++){
        a[k + 8 * (j - entero->n)] = ((entero2->d [j] & (0xF << 4*k)) >> 4*k);
        }
    }
    entero_destruir(entero2);
    return a;
}

bool entero_sumar(entero_t *a, const entero_t *b){// quedo excelente solo hay que ver lo de la mascara (deberia andar con la mascara pero no funca)
    int carry = 0;
    size_t i;
    if(a->n < b->n){ //caso b mas digitos
        if (_redimensionar(a, b->n) == false)
        return false;
    }
    for ( i = 0; i<(a->n); i++){
        if(i<(b->n)){
            uint64_t aux = (uint64_t)a->d[i] + (uint64_t)b->d[i] + carry;
            if ((aux & (OVERFLOW_MASK)) != 0){
                carry = 1;
            }
            else{
                carry = 0;
            }
            a->d[i] = (aux & PRIMEROS_32_DIGITOS);
        }
        else{
            uint64_t aux = (uint64_t)a->d[i] + carry;
            if ((aux & (OVERFLOW_MASK)) != 0){
                carry = 1;
            }
            else 
                carry = 0;
            a->d[i]= (aux & PRIMEROS_32_DIGITOS);
        }
    }
    if (carry != 0){
        if(_redimensionar(a, (a->n) + 1) == false)
            return false;
        a->d [a->n - 1] = carry;
    }
    return true;
}

bool entero_restar(entero_t *a, const entero_t *b){// literalmente el anterior pero con un menos
    if(entero_comparar(a,b) < 0){
        return false;
    }
    size_t i;
    
    for(i = 0; i < a->n; i++){
        if(a->d[a->n - i - 1] == 0){
            continue;
        }
        break;
    }

    if(!_redimensionar(a, (a->n) - i)){ //
        return false;
    }

    int carry = 0;
    
    for (i = 0; i<(a->n); i++){
        if(i<(b->n)){
            uint64_t aux = (uint64_t)a->d[i] - (uint64_t)b->d[i] - carry;
            if ((aux & OVERFLOW_MASK) != 0){
                carry = 1;
            }
            else{
                carry = 0;
            }
            a->d[i] = (aux & PRIMEROS_32_DIGITOS);
        }
        else{
            uint64_t aux = (uint64_t)a->d[i] - carry;
            if ((aux & OVERFLOW_MASK) != 0){
                carry = 1;
            }
            else{
                carry = 0;
            }
            a->d[i] = (aux & PRIMEROS_32_DIGITOS);
        }
    }
    return true;

}

entero_t *entero_desde_bcd(const char bcd[], size_t n){
    size_t longitud = ceil((ceil(3.32 * n) + n * 4)/32 );
    entero_t *entero = _crear(longitud);
    size_t contador = 0;
    for(size_t i = 0; i < entero->n ; i++){
        for(size_t j = 0; j<8 ; j++){
            entero ->d[entero->n - i -1] |= ((uint32_t)bcd[n - 1 - contador]<<(28 - 4 * j));
            contador ++;
            if(contador == n){
                break;
            }
        }
        if (contador == n){
            break;
        }
    }

    size_t iteraciones = entero->n * 32 - (n * 4);
    int romper =0;
    
    for (size_t i = 0;  i<iteraciones ; i++){
        contador = 0;
        entero_desplazar_derecha(entero);
        for (size_t j = 0; j< entero->n; j++){
            for(size_t k = 0; k<8; k++){
                uint32_t numaux = (entero->d[entero->n - j - 1] & ((0xF) << (28 - 4*k))) >> (28 - 4*k);
                if (numaux > 6){
                    numaux  -=  3;
                    entero->d[entero->n - j - 1] &= ~((0xF) << (28 - 4*k));
                    entero->d[entero->n - j - 1] |= ((numaux) << (28 - 4*k));
                    
                }
                if (contador == n-1){
                    romper = 1;
                    break;
                }
                contador ++;
            }
            if (romper == 1){
                romper = 0;
                break;
            }
        }
        
    }

    return(entero);
}

void imprimir_bcd(const char *bcd, size_t digitos){//dado un arreglo de digitos devuelve el numero en "bcd"
    for(size_t i = 0; i < digitos ; i++){ //correccion aca itero con el size_t
        printf("%d",bcd[digitos - 1 - i]);
        if((digitos - 1 - i) % 3 == 0 && (digitos - 1 - i) != 0 ) 
            putchar('_');
    }
}

void _imprimir(const entero_t *e, const char *pre, const char *post);

bool entero_imprimir(const entero_t *entero){
    if(entero == NULL){
        printf("1");
        return true;
    }
    size_t digitos=0;
    int cont=0; 
    for(size_t i = 0 ; i < entero->n; i++){
        if(entero->d[i] != 0){
            cont = 1;
        }
    }
    if(cont == 0){
        printf("%d",0);
    }
    else{
        char *num = entero_a_bcd(entero, &digitos);    
        imprimir_bcd(num, digitos);
        free(num);
    }
    return true;
}

bool entero_dividir(entero_t *dividendo, const entero_t *divisor, entero_t **resto) { // dividendo mayor que divisor
    if(divisor == NULL || dividendo == NULL) {
        return false;
    }// PORFAVOR CHEQUEA QUE ESTO ESTE BIEN. habria que ponerse de acuerdo con las invariantes.

    /*printf("\n");
    printf("divisor:");
    entero_imprimir(divisor);
    printf("\n");
    printf("dividendo:");
    entero_imprimir(dividendo);
    printf("\n");
    */
    entero_t *d = entero_clonar(divisor);
    entero_t *r = entero_clonar(dividendo);
    entero_t *q = _crear(1);

    if(d == NULL || r == NULL || q == NULL) {
        entero_destruir(d);
        entero_destruir(r);
        entero_destruir(q);
        return false;
    }

    // Podríamos tener una función para desplazar elementos en el arreglo.
    for(size_t i = 0; i < dividendo->n * 32; i++){
        entero_desplazar_izquierda(d);

    }
    for(size_t i = 0; i < dividendo->n * 32; i++) {
        entero_desplazar_izquierda(q);
        entero_desplazar_izquierda(r);
        if(entero_comparar(r, d) >= 0) {
            entero_restar(r, d);
            q->d[0] |= 1;
        }
        
    }

    entero_destruir(d);

    // Podríamos tener una función para desplazar elementos en el arreglo.
    for(size_t i = 0; i < dividendo->n * 32; i++)
        entero_desplazar_derecha(r);

    if (resto != NULL) *resto = r;
    else entero_destruir(r);
    free(dividendo->d);
    dividendo->d = q->d;
    dividendo->n = q->n;
    free(q);
    return true;
}

void _imprimir(const entero_t *e, const char *pre, const char *post) {
    printf("%s", pre);
    for(size_t i = 0; i > e->n; i++)
        printf(" %08x", e->d[i]);
    printf("%s", post);
}

char *leer_bcd(size_t *n) {
    int c;
    size_t i = 0;
    char *bcd = NULL;

    while((c = getchar()) != EOF && c != '\n') {
        if(c < '0' || c > '9'){
            fprintf(stderr,"el ingreso es invalido, solo numeros pls");
            return NULL;
        }
        

        char *aux = realloc(bcd, (i + 1) * sizeof(char));
        if(aux == NULL) {
            free(bcd);
            return NULL;
        }

        bcd = aux;

        bcd[i++] = c - '0';
    }

    for(size_t j = 0; j < i / 2; j++) {
        char aux = bcd[j];
        bcd[j] = bcd[i - j - 1];
        bcd[i - j - 1] = aux;
    }

    for(size_t j = 0; j < i ; j++) {
    }

    *n = i;
    return bcd;
}

void imprimir_resultados(entero_t *division, const entero_t *resto, entero_t *entero1, const entero_t *entero2 ){
    printf("numero 1 = ");
    entero_imprimir(entero1);
    printf("\n");
    printf("numero 2 = ");
    entero_imprimir(entero2);
    printf("\n");
    printf("la division numero1/numero2 es = ");
    entero_imprimir(division);
    printf("\n");
    printf("el resto de la divison es = ");
    entero_imprimir(resto);
    printf("\n");
    printf("la suma es = ");
    entero_sumar(entero1, entero2);
    entero_imprimir(entero1);
    printf("\n");

}

bool entero_multiplicar(entero_t *a, const entero_t *b) {
    entero_t *aa = _crear(1);
    if(aa == NULL) return false;

    entero_t *aux = _crear(a->n + b->n + 1);
    if(aux == NULL) {
        entero_destruir(aa);
        return false;
    }

    // Swapeamos el contenido de a con aa, vamos a guardar el resultado en a
    uint32_t *swap = aa->d;
    aa->d = a->d;
    a->d = swap;
    aa->n = a->n;
    a->n = 1;

    for(size_t i = 0; i < b->n; i++) {
        for(size_t j = 0; j < aa->n; j++) {
            uint64_t m = (uint64_t)aa->d[j] * b->d[i];
            aux->d[j] = m;
            aux->d[j + 1] = m >> 32;

            entero_sumar(a, aux);

            aux->d[j] = 0;
        }
        aux->d[aa->n] = 0;

        // INSISTO con lo que dije en dividir:
        // Podríamos tener una función para desplazar elementos en el arreglo.
        for(size_t j = 0; j < 32; j++)
            entero_desplazar_izquierda(aa);
    }

    entero_destruir(aa);
    entero_destruir(aux);

    return true;
}
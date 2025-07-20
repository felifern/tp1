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

bool entero_desplazar_unidades_derecha(entero_t *e, size_t unidades){// no se puede redimensionar de una, corta por el otro lado digamos(es como dividir por 2^32*i caso der)
    if(e->n == 1 || unidades>= e->n){
        _redimensionar(e, 1);
        e->d[0] = 0;
        return true;
    }

    for(size_t i = 0; i< e->n - unidades; i++){
        e->d[i] = e->d [i + unidades];
    }
    _redimensionar(e,e->n - unidades);
    return true;
}

bool entero_desplazar_unidades_izquierda(entero_t *e, size_t unidades){// estas deplazan 32*i unidades para el lado indicado (es como multiplicar por 2^32*i caso izq)
    if(!_redimensionar(e, e->n + unidades)) return false;
    for(size_t i = 0; i < e->n - unidades; i++){
        e->d[e->n - 1  - i] = e->d [e->n - unidades - 1  - i];
    }// lo corro
    for(size_t i = 0; i < unidades; i++){
        e->d[i] = 0;// ???
    }//pongo 0s donde no hay numeros que nos importan
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
        if(i < b->n){
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
    /*//parece ser que para karatsuba o como se escriba esto romppe el programa
    for(i = 0; i < a->n; i++){
        if(a->d[a->n - i - 1] == 0){
            continue;
        }
        break;
    }

    if(!_redimensionar(a, (a->n) - i)){ //
        return false;
    }
    */
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
    }

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

bool multiplicacion_clasica(entero_t *a, const entero_t *b) {
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

bool entero_multiplicar(entero_t *a, const entero_t *b){// PRIMERO ANTES QUE NADA HABRIA QUE IGUALAR LOS DIGITOS DE LOS NUMEROS (LLENAR CON 0s A IZQ) 
    int limite_karatsuba = 8;
    if (a->n <= limite_karatsuba || b->n <= limite_karatsuba){
        entero_t *b_copy = entero_clonar(b);
        if (!b_copy) return false;
        bool ok = multiplicacion_clasica(a, b_copy);
        entero_destruir(b_copy);
        return ok;
    }
    
    
    
    //supongo que tienen igual longitud, para pensar el programa primero (pero falta esto de arriba)
    //yo creo que esto de la longitud se puede resolver rapido con una terna, pero no tengo muy claro como se usa
    if(a == NULL || b == NULL) return false;

    entero_t *bc = entero_clonar(b); 
    if(bc == NULL){
        return false;
    }

    if(a->n == 0 || bc->n == 0){//no deberia pasar nunca
        _redimensionar(a, 1);
        a->d[0]= 0;
        entero_destruir(bc);
        return true;
    }

    
    //printf("\nlen a: %ld, len b: %ld\n", a->n, bc->n);
    //if(!a->n > b->n ? _redimensionar(b, a->n):_redimensionar(a, b->n)) return false;
    // esta no se si esta bien. cuando funcione la pruebo.
    if(a->n == 1 && bc->n == 1){// deberia pasar la verdad
        uint64_t resultado = (uint64_t)a->d[0] * (uint64_t)bc->d[0];
        if(!_redimensionar(a,2)) return false;
        a->d[0]= (uint32_t)(resultado & 0xFFFFFFFF);
        a->d[1]= (uint32_t)(resultado >> 32);
        a->n = 2;
        entero_destruir(bc);
        return true;
    }

    size_t n = (a->n > bc->n) ? a->n : bc->n;
    if (n % 2 != 0) n++;// con esta funciona
    if (!_redimensionar(a, n) || !_redimensionar(bc, n)) {
        entero_destruir(bc);
    return false;
    }

    entero_t *a_uno = entero_clonar(a);
    if(a_uno == NULL) return false;
    if(!entero_desplazar_unidades_derecha(a_uno,a->n - a->n/2)){ 
        entero_destruir(bc);
        return false;
    }// este resulta tener tamanio a->n/2 - a->n
    
    entero_t *a_cero = _crear(a->n - a->n/2);
    if(a_cero == NULL){
        entero_destruir(a_uno);
        entero_destruir(bc);
        return false;
    }
    for(size_t i = 0; i < a->n - a->n/2; i++){
        a_cero->d[i] = a->d[i];
    }
    
    entero_t *b_uno = entero_clonar(bc);
    if(b_uno == NULL){
        entero_destruir(a_uno);
        entero_destruir(a_uno);
        entero_destruir(bc);

        return false;
    }
    if(!entero_desplazar_unidades_derecha(b_uno,bc->n - bc->n/2)) return false;// aca podria estar haciendo un malloc de 0 tamanio
    
    entero_t *b_cero = _crear(bc->n -bc->n/2);
    if(b_cero == NULL){
        entero_destruir(a_uno);
        entero_destruir(a_cero);
        entero_destruir(b_uno);
        entero_destruir(bc);

        return false;
    }
    for(size_t i = 0; i < bc->n - bc->n/2; i++){
        b_cero->d[i] = bc->d[i];
    }
    //me olvido lo de arriba, tengo b_0,a_0,b_1,a_1

    entero_t *z_cero = entero_clonar(a_cero);
    if(z_cero == NULL){
        entero_destruir(a_uno);
        entero_destruir(a_cero);
        entero_destruir(b_uno);
        entero_destruir(b_cero);
    }
    entero_t *z_dos = entero_clonar(a_uno);
    if(z_dos == NULL){
        entero_destruir(a_uno);
        entero_destruir(a_cero);
        entero_destruir(b_uno);
        entero_destruir(b_cero);
        entero_destruir(z_cero);
    }
    entero_t *z_tres = entero_clonar(a_uno);
    if(z_tres == NULL){
        entero_destruir(a_uno);
        entero_destruir(a_cero);
        entero_destruir(b_uno);
        entero_destruir(b_cero);
        entero_destruir(z_cero);
        entero_destruir(z_dos);
    }
    entero_t *aux = entero_clonar(b_uno);
    if(aux == NULL){
        entero_destruir(a_uno);
        entero_destruir(a_cero);
        entero_destruir(b_uno);
        entero_destruir(b_cero);
        entero_destruir(z_cero);
        entero_destruir(z_dos);
        entero_destruir(z_tres);
    }
    //cuentas
    //si falla aca tengo que destruir: a_uno, a_cero, b_uno, b_cero, z_cero, z_dos, z_tres, z_aux y creo que nada mas
    if(!entero_sumar(z_tres, a_cero)){
        entero_destruir(a_uno);
        entero_destruir(a_cero);
        entero_destruir(b_uno);
        entero_destruir(b_cero);
        entero_destruir(z_cero);
        entero_destruir(z_dos);
        entero_destruir(z_tres);
        entero_destruir(aux);
    }
    if(!entero_sumar(aux, b_cero)){
        entero_destruir(a_uno);
        entero_destruir(a_cero);
        entero_destruir(b_uno);
        entero_destruir(b_cero);
        entero_destruir(z_cero);
        entero_destruir(z_dos);
        entero_destruir(z_tres);
        entero_destruir(aux);
    }
    if(!entero_multiplicar(z_tres, aux)){
        entero_destruir(a_uno);
        entero_destruir(a_cero);
        entero_destruir(b_uno);
        entero_destruir(b_cero);
        entero_destruir(z_cero);
        entero_destruir(z_dos);
        entero_destruir(z_tres);
        entero_destruir(aux);
    }//z_tres estaria
    if(!entero_multiplicar(z_cero, b_cero)){
        entero_destruir(a_uno);
        entero_destruir(a_cero);
        entero_destruir(b_uno);
        entero_destruir(b_cero);
        entero_destruir(z_cero);
        entero_destruir(z_dos);
        entero_destruir(z_tres);
        entero_destruir(aux);
    }//z_cero listo
    if(!entero_multiplicar(z_dos,b_uno)){
        entero_destruir(a_uno);
        entero_destruir(a_cero);
        entero_destruir(b_uno);
        entero_destruir(b_cero);
        entero_destruir(z_cero);
        entero_destruir(z_dos);
        entero_destruir(z_tres);
        entero_destruir(aux);
    }//z_dos listo
    //ahora xy = z2 * B^2m + z1 * B^m + z0
    // z1 = z3-z2-z0 = z3-(z2 + z0)
    // yyy B^i == desplazar a izquierda en nuestro caso m = a->n/2
    entero_t *z_uno = entero_clonar(z_tres);
    entero_t *z_suma = entero_clonar(z_dos);
    
    if(z_uno == NULL){
        entero_destruir(a_uno);
        entero_destruir(a_cero);
        entero_destruir(b_uno);
        entero_destruir(b_cero);
        entero_destruir(z_cero);
        entero_destruir(z_dos);
        entero_destruir(z_tres);
        entero_destruir(aux);
    }
    if(!entero_sumar(z_suma, z_cero)){
    }
    if(!entero_restar(z_uno, z_suma)){// esta da false
        entero_destruir(a_uno);
        entero_destruir(a_cero);
        entero_destruir(b_uno);
        entero_destruir(b_cero);
        entero_destruir(z_cero);
        entero_destruir(z_dos);
        entero_destruir(z_tres);
        entero_destruir(aux);
        entero_destruir(z_uno);
    }
    //if(!entero_restar(z_uno, z_cero)){
    //    entero_destruir(a_uno);
    //    entero_destruir(a_cero);
    //    entero_destruir(b_uno);
    //    entero_destruir(b_cero);
    //    entero_destruir(z_cero);
    //    entero_destruir(z_dos);
    //    entero_destruir(z_tres);
    //    entero_destruir(aux);
    //    entero_destruir(z_uno);
    //}
    if(!entero_desplazar_unidades_izquierda(z_dos, 2 * a->n - a->n)){
        entero_destruir(a_uno);
        entero_destruir(a_cero);
        entero_destruir(b_uno);
        entero_destruir(b_cero);
        entero_destruir(z_cero);
        entero_destruir(z_dos);
        entero_destruir(z_tres);
        entero_destruir(aux);
        entero_destruir(z_uno);
    }
    if(!entero_desplazar_unidades_izquierda(z_uno, a->n - a->n/2)){
        entero_destruir(a_uno);
        entero_destruir(a_cero);
        entero_destruir(b_uno);
        entero_destruir(b_cero);
        entero_destruir(z_cero);
        entero_destruir(z_dos);
        entero_destruir(z_tres);
        entero_destruir(aux);
        entero_destruir(z_uno);
    }
    if(!entero_sumar(z_uno,z_dos)){
        entero_destruir(a_uno);
        entero_destruir(a_cero);
        entero_destruir(b_uno);
        entero_destruir(b_cero);
        entero_destruir(z_cero);
        entero_destruir(z_dos);
        entero_destruir(z_tres);
        entero_destruir(aux);
        entero_destruir(z_uno);
    }
    if(!entero_sumar(z_uno,z_cero)){
        entero_destruir(a_uno);
        entero_destruir(a_cero);
        entero_destruir(b_uno);
        entero_destruir(b_cero);
        entero_destruir(z_cero);
        entero_destruir(z_dos);
        entero_destruir(z_tres);
        entero_destruir(aux);
        entero_destruir(z_uno);
    }
    free(a->d);
    a->d = z_uno->d;
    a->n = z_uno->n;
    z_uno->d = NULL;
    entero_destruir(z_uno);
    entero_destruir(a_uno);
    entero_destruir(a_cero);
    entero_destruir(b_uno);
    entero_destruir(b_cero);
    entero_destruir(z_cero);
    entero_destruir(z_dos);
    entero_destruir(z_tres);
    entero_destruir(z_suma);
    entero_destruir(aux);
    entero_destruir(bc);
    //...
    return true;
}//no se. esta medio raro jajaj. hay que probarlo

bool entero_elevar(entero_t *b, const entero_t *e){// creo que esta, tambien hay que probarla
    entero_t *uno = entero_uno();
    if(uno == NULL) return false;
    entero_t *cero = entero_cero();
    if(cero == NULL){
        entero_destruir(uno);
        return false;
    }
    
    if(entero_comparar(e, cero) == 0){
        entero_destruir(cero);
        free(b->d);
        b->d = uno->d;
        b->n = uno->n;
        free(uno);
        return true;
    }
    if(entero_comparar(e, uno) == 0){
        entero_destruir(uno);
        entero_destruir(cero);
        return true;
    }
    entero_t *dos = _crear(1);
    if(dos == NULL){
        entero_destruir(uno);
        entero_destruir(cero);
        return false;
    }
    dos->d[0] = 2;
    dos->n = 1;
    entero_t *bcpy = entero_clonar(b);
    if(bcpy == NULL){
        entero_destruir(uno);
        entero_destruir(cero);
        entero_destruir(dos);
        return false;
    }
    entero_t *ecpy = entero_clonar(e);
    if(ecpy == NULL){
        entero_destruir(uno);
        entero_destruir(cero);
        entero_destruir(dos);
        entero_destruir(bcpy);
        return false;
    }
    entero_t *ecpy2 = entero_clonar(e);
    if(ecpy2 == NULL){
        entero_destruir(uno);
        entero_destruir(cero);
        entero_destruir(dos);
        entero_destruir(bcpy);
        entero_destruir(ecpy);
        return false;
    }
    if(!entero_dividir(ecpy, dos, NULL)){
        entero_destruir(uno);
        entero_destruir(cero);
        entero_destruir(dos);
        entero_destruir(bcpy);
        entero_destruir(ecpy);
        entero_destruir(ecpy2);
        return false;
    }
    if(!entero_restar(ecpy2, ecpy)){
        entero_destruir(uno);
        entero_destruir(cero);
        entero_destruir(dos);
        entero_destruir(bcpy);
        entero_destruir(ecpy);
        entero_destruir(ecpy2);
        return false;
    }
    if(!entero_elevar(b, ecpy)){
        entero_destruir(uno);
        entero_destruir(cero);
        entero_destruir(dos);
        entero_destruir(bcpy);
        entero_destruir(ecpy);
        entero_destruir(ecpy2);
        return false;
    }
    if(!entero_elevar(bcpy, ecpy2)){
        entero_destruir(uno);
        entero_destruir(cero);
        entero_destruir(dos);
        entero_destruir(bcpy);
        entero_destruir(ecpy);
        entero_destruir(ecpy2);
        return false;
    }
    if(!entero_multiplicar(b, bcpy)){
        entero_destruir(uno);
        entero_destruir(cero);
        entero_destruir(dos);
        entero_destruir(bcpy);
        entero_destruir(ecpy);
        entero_destruir(ecpy2);
        return false;
    }
    entero_destruir(uno);
    entero_destruir(cero);
    entero_destruir(dos);
    entero_destruir(ecpy);
    entero_destruir(ecpy2);
    entero_destruir(bcpy);

    return true;
}

bool entero_es_par(const entero_t *e){
    entero_t *dos = _crear(1);
    if(dos == NULL) return false;
    dos->d[0] = 2;
    entero_t *ec = entero_clonar(e);
    if(ec == NULL){
        entero_destruir(dos);
        return false;
    }
    entero_t *cero = entero_cero();
    if(cero == NULL){
        entero_destruir(dos);
        entero_destruir(ec);
        return false;
    }
    entero_t *uno = entero_uno();
    if(uno == NULL){
        entero_destruir(cero);
        entero_destruir(dos);
        entero_destruir(ec);
        return false;
    }
    while(entero_comparar(cero, ec) != 0){
        if(entero_comparar(uno, ec)){
            entero_destruir(cero);
            entero_destruir(uno);
            entero_destruir(dos);
            entero_destruir(ec);
            return false;
        }
        if(!entero_dividir(ec, dos, NULL)){
            entero_destruir(cero);
            entero_destruir(uno);
            entero_destruir(dos);
            entero_destruir(ec);
            return false;
        }
    }
    return true;

}


bool entero_factorial(entero_t *n){
    printf("→ Calculando factorial de: ");
    entero_imprimir(n);
    printf("\n");

    if(n == NULL) return false;
    entero_t *cero = entero_cero();
    if(cero == NULL) return false;
    entero_t *uno = entero_uno();
    if(uno == NULL){
        entero_destruir(cero);
        return false;
    }
    if(entero_comparar(n,  cero) == 0 || entero_comparar(n,  uno) == 0){
        n->n = 1;
        n->d[0] = uno->d[0];
        entero_destruir(uno);
        entero_destruir(cero);
        return true;
    }
    else{
        entero_t *nc = entero_clonar(n);
        if(nc == NULL){
            entero_destruir(uno);
            entero_destruir(cero);
            return false;
        }
        if(!entero_restar(nc,uno)){
            entero_destruir(uno);
            entero_destruir(cero);
            entero_destruir(nc);
            return false;

        }
        if(!entero_factorial(nc)){
            entero_destruir(uno);
            entero_destruir(cero);
            entero_destruir(nc);
            return false;

        }
        if(!entero_multiplicar(n, nc)){
            entero_destruir(uno);
            entero_destruir(cero);
            entero_destruir(nc);
            return false;

        }
        entero_destruir(cero);
        entero_destruir(uno);
        entero_destruir(nc);
        return true;
    }
}// hay que implementar. (feli) HAY QUE HACER LAS VERIFICACIONES PERTINENTES cualquiera.

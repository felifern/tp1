#ifndef OPERADOR_H
#define OPERADOR_H
// lamentablemente llegue a la conclusion de que esto es necesario

typedef struct{
    char *operador;
    //void (*funcion)(void **argumentos); esto estaba asi pero si suponemos que ser reciben racional_t* y devolvemos racional_t*
    racional_t *(*funcion) (racional_t **a);//podemos hacero esto.
    int aridad;
    int prioridad;
    char *descripcion;
}operador_t;




// mmmmmmmmmmm. la idea seria pasar argumentos con void** cada funcion particular sabe que argumentos usar.
// dejo un ejemplo en el main.c 


//comentario anterior:
//Si agrego void *funcion deberia hacerse algun wrapper para poder englobar todo. (creo)
/*
el tema del wrapper es para ver como pasar del void * al puntero a la funcion que necesitamos usar.
pero no debe haber muchos tipos de funciones para definir. (en realidad no se, imagino que debe haber a los sumo 4 o 6)
*/

// entonces en la pila apilamos elemntos_t y si son operadores o funciones podemos buscarlos en al lista que nos
// obligan a hacer. 

// capaz hacer el elemento.h es innecesario, se puede definir todas esas estructuras aca. 
// queda mas desprolijo supongo, pero es mas rapido(?.


#endif
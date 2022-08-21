#ifndef AUTOMATA_H__
#define AUTOMATA_H__

#include "mystring.h"
#include "array.h"

// Tipo incompleto para automatas para manejo con apuntadores por el usuario.
typedef struct automata_t automata_t;

/*
Vease la definicion del automata para entender de que tipo debe ser cada uno
de los elementos que se le pasan al constructor
*/
#define ty array_t

/*
Crea un nuevo objeto automata segun su definicion como una 5-tupla
compuesta por los estados, el alfabeto, la funcion de transicion, el estado
inicial, un arreglo que contiene los estados finales validos y una funcion
que se ejecutara mediante un apuntador en cada paso del automata.
Cada paso se obtiene de un arreglo de estados desde la funcion Delta, por ello,
si se regresa un arreglo de 0 elementos, la funcion step no se ejecuta.
Esto da libertad para implementar estados de error u omitirlos simplemente.
Usa la funcion a la que malloc_fun apunta, (apuntador global).
*/
automata_t*
Automata(ty* states, strset_t* alph, ty* (*Delta)(void*, const char*), 
void* initial_state, ty* final_states, void (*step)(str_t*, size_t, void*));

/*
Libera el automata usando la funcion a la que free_fun apunta, (apuntador
global), no libera recursivamente; es decir no afecta a los elementos de la
estructura, solo el espacio que esta usa para almacenarlos.
*/
void del_automata(automata_t* automata);

/*
Determina el estado final al que llega un AFD dada una cadena expr formada con
el alfabeto de entrada. No usar con AFN.
*/
ty* determine_state(automata_t* automata, str_t* expr);

/*
Determina si un AFD llega a un estado de aceptacion dada una expresion formada
con el alfabeto de entrada, y el arreglo de estados de aceptacion pasados al
constructor del automata. No usar con AFN.
Regresa 0 si llega a un estado no valido, y 1 si llega a un estado valido.
*/
int determine_acceptance(automata_t* automata, str_t* expr);

/*
Recorre todos los caminos del automata (AFN) dada una cadena expr
Ejecuta la funcion a la que step apunta en cada paso que toma el automata.
*/
void traverse_all_paths(automata_t* automata, str_t* expr);

/*
Busca si un estado se encuentra en los estados finales de aceptacion
regresa 0 en caso negativo, y 1 en caso positivo.
*/
int state_in_final(automata_t* automata, void* state);

/*
Busca si un estado se encuentra en los estados que acepta el automata
regresa 0 en caso negativo, y 1 en caso positivo.
*/
int state_in_accepted(automata_t* automata, void* state);

#endif //AUTOMATA_H__

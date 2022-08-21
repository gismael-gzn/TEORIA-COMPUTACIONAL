#ifndef AUTOMATA_H__
#define AUTOMATA_H__

#include "mystring.h"

/*
El macro #ty tiene dos razones de existir:
1. Acortar un poco el tamaño gigantesco del prototipo para crear un automata
2. Dar genericidad a la funcionalidad del automata. Por ahora, #ty esta definido
	para que los estados del automata sean objetos de cualquier tipo definido
	por el usuario, asi, cuando se llame la funcion Delta, se puede hacer que
	el objeto estado "haga algo", se deja a la abstraccion del usuario.
*/

#define ty void

// Tipo incompleto para automatas para manejo con apuntadores por el usuario.
typedef struct automata_t automata_t;


/*
Crea un nuevo objeto automata segun su definicion como una 5-tupla
compuesta por los estados, el alfabeto, la funcion de transicion, el estado
inicial, y el estado final
*/
automata_t* Automata(ty* states, strset_t* alph, ty* (*Delta)(ty*, const char*), 
	ty* initial_state, ty* final_state);

// Libera el objeto automata, sin afectar los objetos que compenen la 5-tupla
void del_automata(automata_t* automata);



// Regresa el estado al que llega un automata con una expresion expr. 
ty* determine_state(automata_t* automata, str_t* expr);

// Regresa 1 si el automata llega a un estado de aceptacion con expr, si no 0
int determine_acceptance(automata_t* automata, str_t* expr);

#endif //AUTOMATA_H__

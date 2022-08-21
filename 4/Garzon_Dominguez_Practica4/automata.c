#include "automata.h"

/*
Definicion de un automata como una 5-tupla, los estados son representados como
objeto(s) de un tipo definido por el usuario. Por ahora solo se tiene la forma
mas sencilla de un automata; se asume que el subconjunto de estados finales es
de tamaño 1, y el conjunto de estados no se usa los metodos del automata,
solo se incluye para representarlo correctamente segun su definicion matematica.

Esta vez se expandio la definicion del automata para que tenga mas manejabilidad
desde el punto de vista programatico; la funcion step se ejecutara en cada paso
que tome el automata, cuando un mapeo de delta regresa un conjunto vacio, la
funcion step no se ejecuta.
*/
struct automata_t
{
	strset_t* alphabet;
	ty
	*states,
	*(*Delta)(void* dom_range, const char*),
	*initial_state,
	*final_state;
	void (*step)(str_t*, size_t, void*);
};



/*
Crea un automata dados los elementos definidos por la 5-tupla.
Y una funcion step, que se ejecuta en cada paso que toma el automata,
*/
automata_t*
Automata(ty* states, strset_t* alph, ty* (*Delta)(void*, const char*), 
void* initial_state, ty* final_states, void (*step)(str_t*, size_t, void*))
{
	automata_t* automata = malloc_fun(sizeof *automata);
	*automata = 
	(automata_t){alph, states, Delta, initial_state, final_states, step};

	return automata;
}

// Libera un automata sin afectar los elementos que lo componen
void del_automata(automata_t* automata)
{
	free_fun(automata);
}

static void traverse_path
(automata_t* automata, void* curr_state, str_t* expr, size_t curr_idx);

/*
Funcion recursiva para recorrer todos los caminos de un AFN, para cada conjunto
de estados que se reciba segun los mapeos de Delta.
Si un estado se indetermina aka el conjunto de estados resultado esta vacio,
la funcion saldra normalmente sin ejecutar la funcion step.
*/
static void traverse_path
(automata_t* automata, void* curr_state, str_t* expr, size_t curr_idx)
{
	if(curr_idx != str_len(expr))
	{
		array_t* states = automata->Delta(curr_state, str_arr(expr)+curr_idx);
		for(size_t j=0, t=get_size(states); j<t; ++j)
		{
			void* item = get_item(states, j);
			automata->step(expr, curr_idx, item);

			traverse_path(automata, item, expr, curr_idx+1);
		}
	}
}

/*
Recorre todos los caminos de un AFN dada un expresion construida sobre el
alfabeto de entrada, o se espera que la funcion delta se haga cargo de determinar
el estado para argumentos no validos. Se llama a la funcion step una vez antes
de recorrer los caminos, para simular el estado de entrada.
*/
void traverse_all_paths(automata_t* automata, str_t* expr)
{
	automata->step(expr, 0, automata->initial_state);
	traverse_path(automata, automata->initial_state, expr, 0);
}

/*
Determina el estado al que llega un automata en particular un AFD
se asume que expr esta constituida por simbolos del alfabeto de entrada.
Se asume que cada mapeo de la funcion delta regresa solo un nuevo estado.
*/
ty* determine_state(automata_t* automata, str_t* expr)
{
	ty* curr_state = automata->initial_state;

	for(size_t i=0, n=str_len(expr); i<n; i++)
	{
		const char* as_arr = str_arr(expr)+i;
		curr_state = automata->Delta(curr_state, as_arr);
	}
	automata->Delta(curr_state, str_arr(expr));

	return curr_state;
}

/*
Busca si un estado se encuentra en la lista de estados pasada
regresa 0 en caso negativo, y 1 en caso positivo.
*/
static int state_in(ty* list, void* state)
{
	enum acceptance {not_accepted=0, accepted} rv=not_accepted;
	for(size_t i=0, n=get_size(list); i<n; ++i)
		if(state == get_item(list, i))
		{
			rv = accepted;
			break;
		}

	return rv;
}

/*
Busca si un estado se encuentra en los estados finales de aceptacion
regresa 0 en caso negativo, y 1 en caso positivo.
*/
int state_in_final(automata_t* automata, void* state)
{
	return state_in(automata->final_state, state);
}

/*
Busca si un estado se encuentra en los estados que acepta el automata
regresa 0 en caso negativo, y 1 en caso positivo.
*/
int state_in_accepted(automata_t* automata, void* state)
{
	return state_in(automata->states, state);
}

/*
Determina si el automata llega a un estado de aceptacion dada una expresion,
se asume que la expresion esta construida sobre el alfabeto de entrada. y que
el automata es finito determinista
*/
int determine_acceptance(automata_t* automata, str_t* expr)
{
	ty* final_state = determine_state(automata, expr);
	return state_in_final(automata, get_item(final_state, 0));
}


#undef ty

#include "automata.h"

/*
Definicion de un automata como una 5-tupla, los estados son representados como
objeto(s) de un tipo definido por el usuario. Por ahora solo se tiene la forma
mas sencilla de un automata; se asume que el subconjunto de estados finales es
de tamaño 1, y el conjunto de estados no se usa los metodos del automata,
solo se incluye para representarlo correctamente segun su definicion matematica.
*/
struct automata_t
{
	strset_t* alphabet;
	ty
	*states,
	*(*Delta)(ty* dom_range, const char* dom),
	*initial_state,
	*final_state;
};

// Crea un automata dados los elementos definidos por la 5-tupla
automata_t* Automata(ty* states, strset_t* alph, ty* (*Delta)(ty*, const char*), 
ty* initial_state, ty* final_states)
{
	automata_t* automata = malloc(sizeof *automata);
	*automata = (automata_t){alph, states, Delta, initial_state, final_states};
	return automata;
}

// Libera un automata sin afectar los elementos que lo componen
void del_automata(automata_t* automata)
{
	free(automata);
}

/*
Determina el estado al que llega el automata dada una expresion, se asume que la
expresion esta construida sobre el alfabeto de entrada, en caso contrario se
considera comportamiento indefinido aunque probablemente esto cause un segfault
*/
ty* determine_state(automata_t* automata, str_t* expr)
{
	ty* curr_state = automata->initial_state;
	const char* curr_symbol = NULL;

	for(size_t i=0, n=str_len(expr); i<n; i++)
	{
		curr_symbol = str_arr(expr)+i;
		curr_state = automata->Delta(curr_state, curr_symbol);
	}
	automata->Delta(curr_state, curr_symbol);

	return curr_state;
}

/*
Determina si el automata llega a un estado de aceptacion dada una expresion,
se asume que la expresion esta construida sobre el alfabeto de entrada.
*/
int determine_acceptance(automata_t* automata, str_t* expr)
{
	enum acceptance {not_accepted=0, accepted};

	ty* final_state = determine_state(automata, expr);

	// printf("\n %p vs %p\n", final_state, automata->final_state);

	if(final_state == automata->final_state)
		return accepted;
	else
		return not_accepted;
}

#undef ty


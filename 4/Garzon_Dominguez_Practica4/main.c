#include "mystring.h"
#include "automata.h"

void step(str_t* expr, size_t idx, void* obj);
int* dom(int id);
array_t* set(int id);
array_t* delta(void* curr_state, const char* as_arr);
void print_integer(void* p);
array_t* as_arr(int* state_set, int from, int to);
void input(void);

#define ever (;;)

// Para que main y los bufers sean visibles en todo el programa.
struct main_state
{
	int bufsiz;
	str_t* expr;
	array_t* path;
	char in_buf[4096], out_buf[4096];
}Main = {4096, NULL, NULL, {0}, {0}};

int main(int argc, char const *argv[])
{

	array_t *st = as_arr(dom(0), 0,5), *fst = as_arr(dom(0), 1,4);
	automata_t* automaton = Automata(st, NULL, delta, dom(0), fst, step);

	printf("Implementacion automata ejemplo 1. Con manejo de error basico\n");
	printf("Escriba \"_\" para salir\n");

	for ever
	{
		input();

		if(Main.in_buf[0] == '_')
			break;
		else
			traverse_all_paths(automaton, Main.expr);

		str_del(Main.expr), array_del(Main.path);
		Main.path = NULL;
	}

	array_del(st), array_del(fst), del_automata(automaton), set(-1);
	str_del(Main.expr);

	return 0;
}


/*
Dominio de la funcion delta, incluye un estado de error identificado con -1
El resto de estados son identificados con un numero entero.
*/
int* dom(int id)
{
	static int called = 0;
	static int states[6] = {0};

	if(called == 0)
	{
		++called;
		for(int i=0; i<5; ++i)
			states[i] = i;

		states[5] = -1;
	}

	return states+id;
}

// Regresa una lista de estados identificada por id
array_t* set(int id)
{
	static array_t* states_map[10] = {0};

	if(*states_map == NULL)
	{
		static int code[][3] = { {2, 1,4}, {1, 3}, {1, 1}, {1, 2}, {1, 5}, 
		{1, 5}, {1, 5}, {1, 5}, {1, 5}, {1, 4}, };

		for(int i=0; i<10; ++i)
		{
			states_map[i] = array_new(1);
			for(int j=0, n=*code[i]; j<n; ++j)
				array_add(states_map[i], dom(code[i][j+1]));
		}
	}

	if(id == -1)
		for(int i=0; i<10; ++i)
			array_del(states_map[i]);

	return states_map[id];
}

/*
Funcion delta, manejo muy basico de error para cadenas no validas, o para
estados indetermindos? (cuando existe un conjunto vacio)
*/
array_t* delta(void* curr_state, const char* as_arr)
{
	const char opt = *as_arr;
	enum {erri = 5, errf = 8};

	if(opt != 'a' && opt != 'b')
		return set(5);

	if(curr_state == dom(0))
		switch(opt) {
			case 'a': return set(0); case 'b': return set(1);
		}
	else if(curr_state == dom(1))
		switch(opt) {
			case 'a': return set(2); case 'b': return set(3);
		}
	else if(curr_state == dom(2))
		switch(opt) {
			case 'a': return set(erri); case 'b': return set(erri);
		}
	else if(curr_state == dom(3))
		switch(opt) {
			case 'a': return set(erri); case 'b': return set(erri);
		}
	else if(curr_state == dom(4))
		switch(opt) {
			case 'a': return set(erri); case 'b': return set(9);
		}
	else if(curr_state == dom(5))
		switch(opt) {
			default: return set(erri);
		}

	return NULL;
}


// Imprime entero
void print_integer(void* p)
{
	int v = *(int*)p;
	const char* msg = empty_string;
	if(v == -1)
		msg = "Error ";
	printf(">>(%s%d)", msg, v);
}

/*
La funcion step para cada paso del automata, contiene un arreglo para almacenar
el camino actual que se esta atravesando, por regla general ningun camino sera
mayor que |expr|+1 (si contamos el estado de entrada). Por la manera en que fue
escrita la funcion traverse_all_paths, se puede mantener registro de los caminos
sin ocupar demasiada memoria, si es necesario, se puede agregar mas a step, para
que al detectar caminos validos, se guarde tal camino u otras acciones. 
*/
void step(str_t* expr, size_t idx, void* obj)
{
	if(Main.path == NULL)
	{
		Main.path = array_new(str_len(expr)+1);
		set_item(Main.path, 0, obj);
	}	
	else
	{
		set_item(Main.path, idx+1, obj);
		if(idx == str_len(expr)-1)
		{	
			const char* msg = "Camino invalido";
			if(obj >= (void*)dom(2) && obj <= (void*)dom(4))
				msg = "(*!)  Camino valido";

			printf("%s \"", msg);
			str_write(expr, stdout);
			printf("\":\n");

			for_each_in(Main.path, print_integer);
			printf("\n\n");
		}
	}	
}

// Convierte arreglo de estados a un arreglo dinamico en el rango from a to
array_t* as_arr(int* state_set, int from, int to)
{
	array_t* rv = array_new(to-from);

	for(; from<to; ++from)
		set_item(rv, from, state_set+from);

	return rv;
}

// Recibe entrada del usuario
void input(void)
{
	printf(">> ");
	fgets(Main.in_buf, Main.bufsiz, stdin);
	Main.expr = strn_new(Main.in_buf, my_strlen(Main.in_buf)-1);
}
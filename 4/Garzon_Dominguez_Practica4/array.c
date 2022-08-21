#include "array.h"

// Tipo completo que describe un arreglo generico
struct array_t
{
	ARRAY_TYPE(void*)
};

/*
No hay mucho que explicar aqui, solo se expanden los macros para arreglo
dinamico desde data_structures/array.h, claramente los elementos del arreglo
generico son apuntadores a algun objeto previamente definido en el codigo.

Para las funciones de memoria se usaran los apuntadores declarados en 
data_structures/container.h, (simbolos externos/globales) que estan definidos
posteriormente en string.c, asi que las funciones malloc, realloc, memsert, etc
son las equivalentes a las que se encuentran definidas en string.c
*/

array_t* array_new(size_t initial_size)
{
	if(initial_size == 0)
		initial_size = 1;

	array_t* array = malloc_fun(sizeof *array);
	a_init_siz(array, initial_size);

	return array;
}

void array_del(array_t* array)
{
	a_del(array);
	free_fun(array);
}

void array_add(array_t* array, void* element)
{
	a_add(array, element);
}

size_t get_size(array_t* array)
{
	return get_len(array);
}

void set_item(array_t* array, size_t n, void* element)
{
	if(n == get_size(array))
		array_add(array, element);
	else
		a_idx(array, n) = element;
}

void* get_item(array_t* array, size_t n)
{
	return a_idx(array, n);
}

void* del_item(array_t* array, size_t n)
{
	void* rv = get_item(array, n);
	a_pop(array, n);
	return rv;
}

void for_each_in(array_t* array, void (*do_this)(void* with_element))
{
	if(do_this != NULL)
		for(array(array, void**, i))
			do_this(*i);
}


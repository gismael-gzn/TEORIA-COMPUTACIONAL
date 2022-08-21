#include "array.h"

// Tipo completo que describe un arreglo generico
struct array_t
{
	ARRAY_TYPE(void*)
};

size_t ptr_cmp(void* p1, void* p2);

size_t ptr_cmp(void* p1, void* p2)
{
	return !(p1 == p2);
}

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

void array_grow(array_t* array, size_t new_elements)
{
	a_grow(array, new_elements)
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

size_t get_avail(array_t* array)
{
	return a_slots(array);
}

void set_item(array_t* array, size_t n, void* element)
{
	size_t siz = get_size(array);
	if(n == siz)
		a_add(array, element)
	else
		a_idx(array, n) = element;
}

void** find_item
(array_t* array, void* item, size_t from, size_t (*cmp)(void*, void*))
{
	if(cmp == NULL)
		cmp = ptr_cmp;

	void** offset = NULL;

	for(void** p = a_get(array, from); a_auto(array, p); ++p)
		if(cmp(item, *p) == 0)
		{
			offset = p;
			break;
		}

	return offset;
}

size_t get_index(array_t* array, void** pos)
{
	return idx(array, pos);
}


const void* get_memblock(array_t* array)
{
	return arr(array);
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


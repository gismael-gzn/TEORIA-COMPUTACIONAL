#ifndef GENERIC_ARRAY_H__
#define GENERIC_ARRAY_H__

// Utilidades para crear arreglos dinamicos de tipo generico
#include "data_structures/array.h"

// Tipo incompleto para usar mediante apuntadores
typedef struct array_t array_t;

// Crea un nuevo arreglo con un tamaño inicial de initial_size elementos
array_t* array_new(size_t initial_size);

// Crece un arreglo agregando espacio suficiente para new_elements
void array_grow(array_t* array, size_t new_elements);

// Libera el bloque de memoria que contiene el arreglo de apuntadores void
void array_del(array_t* array);

// Agrega un elemento al final del arreglo
void array_add(array_t* array, void* element);

// Regresa el elemento n dentro del arrreglo (de manera circular)
void* get_item(array_t* array, size_t n);

/*
Busca un item dentro del arreglo dada una funcion de comparacion la cual
espera 0 en el de que dos items sean iguales, si se encuentra, se regresa
un apuntador a una posicion del bloque de memoria (a partir de este, se puede
calcular el indice correspondiente al elemento, para usar con llamadas
consecutivas.
Se regresa NULL, si no se encuentra el elemento.
Si cmp es NULL, se usara simple comparacion de apuntadores.
*/
void** find_item
(array_t* array, void* item, size_t from, size_t (*cmp)(void*, void*));

// Regresa el indice al que apunta pos dentro del arreglo.
size_t get_index(array_t* array, void** pos);

// Regresa el bloque de memoria en el que se contiene el arreglo.
const void* get_memblock(array_t* array);

/*
Establece el n-esimo elemento circular del arreglo al apuntador que se le pasa
si n == get_size(array), se agrega el elemento al final.
*/
void set_item(array_t* array, size_t n, void* element);

// Elimina y regresa el n-esimo elemento circular del arreglo
void* del_item(array_t* array, size_t n);

// Devuelve la cantidad de elementos en el arreglo
size_t get_size(array_t* array);

// Devuelve la cantidad de espacio disponible en terminos de elementos
size_t get_avail(array_t* array);

// Ejecuta do_this(with_element), para cada elemento del arreglo
// Si do_this == NULL, no sucede nada.
void for_each_in(array_t* array, void (*do_this)(void* with_element));


#endif //GENERIC_ARRAY_H__

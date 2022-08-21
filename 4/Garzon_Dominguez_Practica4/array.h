#ifndef GENERIC_ARRAY_H__
#define GENERIC_ARRAY_H__

#include "data_structures/array.h"

// Utilidades para crear arreglos dinamicos de tipo generico

// Tipo incompleto para usar mediante apuntadores
typedef struct array_t array_t;

// Crea un nuevo arreglo con un tamaño inicial de initial_size elementos
array_t* array_new(size_t initial_size);

// Libera el bloque de memoria que contiene el arreglo de apuntadores void
void array_del(array_t* array);

// Agrega un elemento al final del arreglo
void array_add(array_t* array, void* element);

// Regresa el elemento n dentro del arrreglo (de manera circular)
void* get_item(array_t* array, size_t n);

/*
Establece el n-esimo elemento circular del arreglo al apuntador que se le pasa
si n == get_size(array), se agrega el elemento al final.
*/
void set_item(array_t* array, size_t n, void* element);

// Elimina y regresa el n-esimo elemento circular del arreglo
void* del_item(array_t* array, size_t n);

// Devuelve la cantidad de elementos en el arreglo
size_t get_size(array_t* array);

// Ejecuta do_this(with_element), para cada elemento del arreglo
// Si do_this == NULL, no sucede nada.
void for_each_in(array_t* array, void (*do_this)(void* with_element));


#endif //GENERIC_ARRAY_H__
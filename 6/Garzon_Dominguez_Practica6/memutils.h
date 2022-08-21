#ifndef MEMUTILS_H__
#define MEMUTILS_H__

/*
Este modulo sirve para mantener rastro de la memoria de un programa de manera
muy basica. Usa los apuntadores a funcion declarados en 
"data_structures/container.h", para que las funciones que los usan (basicamente
en la mayoria de archivos para este punto) usen los envoltorios de funcion
para malloc, realloc, y free, proveidos en este archivo.
*/

#include <stdio.h>
#include "array.h"

// Banderas para llamar mem_utils
enum mem_utils_flags {free_at_exit, never_free};

// Simbolos externos de este modulo, declarados aqui definidos en memutils.c
extern array_t* utils_memory;
extern size_t utils_allocs, utils_reallocs, utils_frees;

/*
Inicializa el modulo en un modo segun la bandera.
*free_at_exit: liberara la memoria no liberada por free_fun previamente al llegar
al final del programa.
*never_free: no libera la memoria al final del programa, pero mantiene rastro de
los apuntadores a bloques alojados por malloc_fun, dentro de utils_memory
Si no se llama, se asume que los apuntadores deberan apuntar a la implementacion
estandar de malloc, realloc, free.
*no_track, establece los apuntadores a las implementaciones stdlib de malloc,
realloc, y free, puede usarse tras una rutina de limpieza, por ejemplo.
*/
void mem_utils(enum mem_utils_flags flag);

#endif // MEMUTILS_H__


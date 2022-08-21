#ifndef MYSTRING_H__
#define MYSTRING_H__

#include <stdlib.h>
#include <stdio.h>

// Vease el directorio data_structures
#include "data_structures/singly.h"
#include "array.h"

static const char *const empty_string = "";

/*
Protipos para la implementacion propia de metodos de string.h
Los metodos para str_t estan construidos sobre estas funciones
*/
size_t my_strlen(const char* s);
int my_strcmp(const char *s1, const char *s2);
int my_memcmp(const void *s1, const void *s2, size_t n);
void* my_memmove(void *dst, const void *src, size_t n);
void* my_memset(void *b, int c, size_t len);

/*
Separa una cadena str en n tokens insertados en un arreglo con sentinela NULL
cada que se encuentre el separador sep dentro de str. Si seq es la cadena vacia 
o una subcadena inexistente en str, el unico token de la lista es str.
str es modificada con nul por cada inicio de la secuencia completa sep.
str y sep deben ser nul terminadas.
*/
array_t* str_sep(char* str, const char* sep);


// Tipo incompleto para manejar cadenas, solo se puede usar como apuntador
typedef struct str_t str_t;

// Tipo incompleto para manejar conjuntos de cadenas
typedef struct strset_t strset_t;

/*
Lo mismo que str_sep, pero regresa un arreglo de objetos str_t
* No implementada aun *
*/
array_t* str_sep_as_strobj(char* str, char* sep);

/*
Lo misom que str_sep_as_strobj, pero toma objetos de tipo str_t como argumentos.
Los objetos del arreglo son objetos nuevos (no se muta str)
* No implementada aun *
*/
array_t* str_sep_from_strobj(str_t* str, str_t* sep);

// Crea un conjunto de cadenas dado un arreglo con sentinela NULL
strset_t* set_from_arr(const char *array[]);

// Crea un nuevo objeto conjunto
strset_t* set_new(void);

// Agrega la cadena str al set mediante un apuntador
// void set_add(strset_t* set, str_t* sptr);

// Añade un nuevo elemento copiandolo
void set_add_raw(strset_t* set, const char* str);

// Agrega la cadena obj a set mediante un apuntador (copia debil) 
void set_add_ref(strset_t* set, str_t* obj);

// Agrega la cadena sptr copiandola
// void set_add_cpy(strset_t* set, str_t* sptr);

// Busca si el elemento obj existe en el set (segun los contenidos de obj)
str_t* Element_ref(strset_t* set1, str_t* obj);

// Regresa un apuntador al objeto cadena si existe el elemento en el conjunto
str_t* Element(strset_t* set1, const char* elem, size_t l);

// Crea un nuevo conjunto aplicando union de conjuntos entre set 1 y 2
// Crea copias sencillas de los objetos cadena (con apuntador)
strset_t* Union(strset_t* set1, strset_t* set2);

// Crea un nuevo conjunto aplicando interseccion de conjuntos entre set 1 y 2
// Crea copias sencillas de los objetos cadena (con apuntador)
strset_t* Intersection(strset_t* set1, strset_t* set2);

// Crea un nuevo conjunto aplicando diferencia de conjuntos entre set 1 y 2
// Crea copias sencillas de los objetos cadena (con apuntador)
strset_t* Difference(strset_t* set1, strset_t* set2);

// Concatenacion o producto cartesiano entre conjuntos de cadenas set1 y set2
// Crea copias enteras de los objetos cadena (byte a byte)
strset_t* Cartesian(strset_t* set1, strset_t* set2);

// Concatenacion el conjunto set1 consigo mismo pow veces
// Crea copias enteras de los objetos cadena (byte a byte)
strset_t* Power(strset_t* set1, long pow);

// Aplica estrella de kleen sobre set1 desde 0 hasta lim, 
// Crea copias enteras de los objetos cadena (byte a byte)
strset_t* Kleene_star(strset_t* set1, long lim);

// Aplica estrella de kleen sobre set1 desde 0 hasta lim, 
// Crea copias enteras de los objetos cadena (byte a byte)
strset_t* Kleene_plus(strset_t* set1, long lim);

// Regresa el numero de elementos de set
size_t Count(strset_t* set);

// Libera conjuntos que tengan copias sencillas de objetos cadnea
void set_weak_del(strset_t* set);

// Libera conjuntos que tengan copias enteras de objetos cadena
void set_strong_del(strset_t* set);

// Escribe un conjunto a un stream
size_t set_write(strset_t* set, FILE* stream);

// Imprime con formato un conjunto a un stream, agregando al inicio title,
// title no debe ser mayor de 255 elementos
size_t set_writef(strset_t* set, FILE* stream, const char* title);


// Funciones para obtener informacion sobre un conjunto, desde el punto de vista
// de la implementacion (vease el archivo mystring.c)
size_t set_writedbg(strset_t* set);
size_t set_impl_total_buckets(strset_t* set);
size_t set_impl_free_buckets(strset_t* set);
size_t set_impl_used_buckets(strset_t* set);
size_t set_impl_total_elems(strset_t* set);

// Crea una nueva cadena a partir de una sequencia terminada con char nulo
str_t* str_new(const char* str);

// Crea una nueva cadena a partir de n bytes de str
str_t* strn_new(const char* str, size_t n);

// Crea una nueva cadena copiando otro objeto cadena
str_t* str_new_cpy(str_t* sptr);

// Concatena sptr1 y sptr2 en una nueva cadena
str_t* str_new_ccat(str_t* sptr1, str_t* sptr2);

// Concatena sptr1 y str en una nueva cadena
str_t* str_new_ccat_raw(str_t* sptr1, const char* str);

// Regresa la cadena invertida de sptr
str_t* str_new_rev(str_t* sptr);

// Lee maximo max_size caracteres de stdin los carga en una cadena
// Imprime un prompt personalizado en stdout
str_t* str_input(size_t max_size, const char* prompt);

// Regresa el bloque de memoria donde un objeto sptr guarda su cadena
// Este bloque no es nul terminado y no debe ser modificado (note el const)
const char* str_arr(str_t* sptr);

// Usa free para liberar los recursos ocupados por una cadena
void str_del(str_t* sptr);

// Concatena sptr2 a sptr1, muta la cadena original
void str_ccat(str_t* sptr1, str_t* sptr2);

// Concatena un arreglo con terminacion de char nulo a sptr1, mutando a sptr1
void str_ccat_raw(str_t* sptr1, const char* str);

// Compara dos cadenas, regresa 0 si son iguales, otro numero si no los son
int str_cmp(str_t* sptr1, str_t* sptr2);

// Regresa el codigo hash de una cadena.
size_t str_hash(str_t* obj);

// Regresa la longitud de una cadena
size_t str_len(str_t* sptr);

// No es relevante, pero regresa la memoria disponible en una cadena
size_t str_avail(str_t* sptr);

// Regresa el elemento pos%lenght de sptr
char str_idx(str_t* sptr, size_t pos);

// Potencia de una cadena pow veces, si pow es 0 regresa una nueva cadena vacia
str_t* str_pow(str_t* sptr, long pow);

/*
Regresa una cadena en sequencia desde los indices from a to
el valor menor es inclusivo, y los valores mayores o iguales son exclusivos
(como indices dentro de la cadena) ejemplo:
si str = "hello"
str_slice(str, 0, 6) = "hello", str_slice(str, 6, 6) = ""
str_slice(str, 6, 0) = "olleh", str_slice(str, 3,4) = "l"
*/
str_t* str_slice(str_t* sptr, size_t from, size_t to);

// Forma una subsequencia de sptr a partir de una lista de indices
// La funcion se limita a tomar cada indice sin importar repeticion u orden
str_t* str_seq1(str_t* sptr, size_t* idx_pos_list);

// Invierte la cadena sptr mutandola
void str_rev(str_t* sptr);

// Escribe la cadena sptr a stream, regresa el numero de elemtos escritos
size_t str_write(str_t* sptr, FILE* stream);

// Intenta leer n bytes de un stream, los concatena a sptr
size_t str_read(str_t* sptr, FILE* stream, size_t n);

// Toma una cadena como buffer para un stream
int str_buf(str_t* sptr, FILE* stream, size_t bufsize, int mode);


#endif

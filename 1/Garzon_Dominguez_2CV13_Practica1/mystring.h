#ifndef MYSTRING_H__
#define MYSTRING_H__


#include <stdlib.h>
#include <stdio.h>

//Para los apuntadores a funcion, my_malloc, my_free... usar al inicio de main
void string_module_init(void);

static const char *const empty_string = "";

/*
Porotipos implementacion propia de metodos de string.h
Los metodos para str_t estan construidos sobre estas funciones
*/
size_t my_strlen(const char* s);
int my_strcmp(const char *s1, const char *s2);
void* my_memmove(void *dst, const void *src, size_t n);
void* my_memset(void *b, int c, size_t len);

// Tipo incompleto para manejar cadenas, solo se puede usar como apuntador
typedef struct str_t str_t;


// Crea una nueva cadena a partir de una sequencia terminada con char nulo
str_t* str_new(const char* str);

// Crea una nueva cadena copiando otro objeto cadena
str_t* str_new_cpy(str_t* sptr);

// Concatena sptr1 y sptr2 en una nueva cadena
str_t* str_new_ccat(str_t* sptr1, str_t* sptr2);

// Concatena sptr1 y str en una nueva cadena
str_t* str_new_ccat_raw(str_t* sptr1, const char* str);

// Regresa la cadena invertida de sptr
str_t* str_new_rev(str_t* sptr);

// Usa free para liberar los recursos ocupados por una cadena
void str_del(str_t* sptr);

// Concatena sptr2 a sptr1, muta la cadena original
void str_ccat(str_t* sptr1, str_t* sptr2);

// Concatena un arreglo con terminacion de char nulo a sptr1, mutando a sptr1
void str_ccat_raw(str_t* sptr1, const char* str);

// Compara dos cadenas, regresa 0 si son iguales, otro numero si no los son
int str_cmp(str_t* sptr1, str_t* sptr2);

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

// Toma una cadena como buffer par aun stream
int str_buf(str_t* sptr, FILE* stream, size_t bufsize, int mode);


#endif

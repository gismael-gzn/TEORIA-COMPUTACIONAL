#include "mystring.h"

/*
array.h es un pequeño proyecto personal, son una serie de macros que permiten 
crear contenedores de tipo generico, similar al vector de c++
*/
#include "array.h"

// Tipo completo para manejar cadenas
// Mantiene informacion sobre el uso de memoriaver macro en array.h
struct str_t
{
	ARRAY_TYPE(char)
};

// Prototipos para algunas funciones especiales usadas solo en este archivo
int my_strcmp_siz(const char *s1, const char *s2, size_t l1, size_t l2);
void* my_memmove_rev(void *dst, const void *src, size_t n);
void str_ccat_raw_rev(str_t* sptr1, const char* str, size_t len);
void str_ccat_raw_fwd(str_t* sptr1, const char* str, size_t len);

// Envolver malloc y realloc en funciones que chequeen regreso nulo
void* my_malloc(size_t s);
void* my_realloc(void* p, size_t s);
void my_free(void* p);

// Apuntadores a funcion que usan los macros de array.h. No usar string.h
// asi que hacemos que apunten a las implementaciones de este archivo
void string_module_init(void)
{
	malloc_fun = my_malloc, realloc_fun = my_realloc, free_fun = my_free,
	memset_fun = my_memset, memmove_fun = my_memmove;
}

const char *const nomem_err = 
"sin memoria disponible... continuando...";

// Malloc que avisa a stderr si malloc falla, el programa continua
void* my_malloc(size_t s)
{
	// assert(s != 0);
	void* c = malloc(s);
	if(c == NULL)
		fprintf(stderr, "%s\n", nomem_err);
	return c;
}

// Realloc que avisa a stderr si realloc falla, el programa continua
void* my_realloc(void* p, size_t s)
{
	// assert(s != 0);
	void* c = realloc(p, s);
	if(c == NULL)
		fprintf(stderr, "%s\n", nomem_err),
		c = p;
	return c;
}

// free
void my_free(void* p)
{
	free(p);
}

// Calcula la longitud de una sequencia de chars(1byte) terminada con char nulo
size_t my_strlen(const char* s)
{
	size_t len = 0;
	while(*s++ != '\0')
		++len;
	return len;
}

// Compara mientras los caracteres ean iguales y ninguno sea caracter nulo
int my_strcmp(const char *s1, const char *s2)
{
	while( *s1 == *s2 && (*s1 & *s2) != '\0')
		++s1, ++s2;

	return *s1 - *s2;
}

// Compara dadas las cadenas s1, s2 y sus longitudes
// Regresa 0 si son iguales, si no, regresa otro numero
int my_strcmp_siz(const char *s1, const char *s2, size_t l1, size_t l2)
{
	if(s1 == s2)
		return 0;

	if(l1 == l2)
		for(; *s1 == *s2 && l1 != 0; ++s1, ++s2, --l1);
	else
		++l1;

	return l1;
}

// Copia n bytes de src a dst incluso si translapan
// En teoria deberia funcionar incluso si dst y src no apuntan al mismo bloque
void* my_memmove(void *dst, const void *src, size_t n)
{
	if(dst == src)
		return dst;

	char *srcc=(char*)src, *dstc=dst;
	int dir=1;

	if(dstc > srcc)
		dir = -1, srcc += n-1, dstc += n-1;

	while(n-- != 0)
		*dstc = *srcc,
		dstc+=dir, srcc+=dir;

	return dst;
}

// Copia n bytes de src en reversa y hacia adelante en dst
void* my_memmove_rev(void *dst, const void *src, size_t n)
{
	char *dstc = dst, *srcc = (char*)src;
	while(n-- != 0)
		*dstc++ = *srcc--;

	return dst;
}

// Lo que hace memset de string,h
void* my_memset(void *b, int c, size_t len)
{
	char* bp = b;
	while(len-- != 0)
		*bp++ = c;
	return b;
}

// Concatena str a sptr1 usando una funcion memmove en particular
void str_ccat_raw_rev(str_t* sptr1, const char* str, size_t len)
{
	a_ccat_raw_with(sptr1, str, len, my_memmove_rev)
}

// Concatena len bytes de str a sptr1 hacia enfrente
void str_ccat_raw_fwd(str_t* sptr1, const char* str, size_t len)
{
	a_ccat_raw(sptr1, str, len)
}


/*
Metodos para str_t
Se asume que cada vez que se recibe un const char* este tiene terminacion
Con caracter nulo. La mayoria de las funciones estan construidas sobre macros
de array.h, estos se encargan de mantener registro del tamaño del arreglo,
memoria disponible en el arreglo y cuando es necesario alojar o liberar memoria
dichos macros necesitan usar implementaciones de funciones como memmove.
Mediante apuntadores a funciones, se especifica que se usara la implementacion
de dichos metodos se encuentra en este archivo (asi evitamos usar string.h)

varias funciones para cadenas como strdup, strcat, strcpy, pueden ser escritas
en terminos de memmove, por eso es que dichas funciones no estan implementadas
en este archivo para const char*, sino que se usan los macros de array.h,
para modificar un contenedor str_t que "maneja su memoria"

un par de notas:
* Los macros que reciben posiciones de indices, entre ellos: a_get, a_idx
	no chequeen si el indice esta fuera de rango, en su lugar toman el
	modulo del indice respecto a la longitud del arreglo, (tambien evitando la
	excepcion de punto flotante si len es 0), basicamente es arreglo circular
* my_memcpy podria no ser funcional, debido a que comparar apuntadores que no
	apunten al mismo bloque de memoria es comportamiento indefinido, sin embargo
	asumimos que el compilador se encargara de eso
*/

// Toma la longitud de str para copiar al contenedor str_t
str_t* str_new(const char* str)
{
	str_t* string = my_malloc(sizeof *string);
	a_init(string)
	str_ccat_raw_fwd(string, str, my_strlen(str));
	return string;
}

// Copia una copia del contenedor str_t
str_t* str_new_cpy(str_t* sptr)
{
	str_t* string = str_new(empty_string);
	str_ccat(string, sptr);
	return string;
}

// Crea un contenedor con los contenidos de sptr1 y sptr2 en sequencia
str_t* str_new_ccat(str_t* sptr1, str_t* sptr2)
{
	str_t* string = str_new(empty_string);
	str_ccat(string, sptr1),
	str_ccat(string, sptr2);
	return string;
}

// Toma la longitud de str para agregar los bytes al contenedor sptr1
str_t* str_new_ccat_raw(str_t* sptr1, const char* str)
{
	str_t* string = str_new_cpy(sptr1);
	str_ccat_raw_fwd(string, str, my_strlen(str));
	return string;
}

// Copia el contenedor sptr1 en reversa
str_t* str_new_rev(str_t* sptr)
{
	str_t* string = str_new(empty_string);
	size_t from = get_len(sptr) - (get_len(sptr)==0? 0: 1);
	str_ccat_raw_rev(string, a_get(sptr, from), get_len(sptr));
	return string;
}

// Libera los contenidos de sptr y el apuntador de sptr 
void str_del(str_t* sptr)
{
	a_del(sptr)
	my_free(sptr);
}

// Concatena sptr2 a sptr1, sin crear copias
void str_ccat(str_t* sptr1, str_t* sptr2)
{
	a_ccat(sptr1, sptr2)
}

// Concatena str a sptr1 sin crear copias
void str_ccat_raw(str_t* sptr1, const char* str)
{
	str_ccat_raw_fwd(sptr1, str, my_strlen(str));
}

// Compara los contenidos de sptr1 y sptr2
int str_cmp(str_t* sptr1, str_t* sptr2)
{
	return my_strcmp_siz(arr(sptr1), arr(sptr2), get_len(sptr1), get_len(sptr2));
}

// Regresa los elementos ocupados en el arreglo de sptr
size_t str_len(str_t* sptr)
{
	return get_len(sptr);
}

// Regresa los elementos disponibles en el arreglo de sptr
size_t str_avail(str_t* sptr)
{
	return a_slots(sptr);
}

// Regresa el elemento situado en el indice pos modulado
char str_idx(str_t* sptr, size_t pos)
{
	return a_idx(sptr, pos);
}

/*
Potencia de una cadena, se manejan ambos casos, pow>0 y pow<0, usando variables
para controlar, que funcion se llama, y como varia pow en el ciclo while
*/
str_t* str_pow(str_t* sptr, long pow)
{
	str_t* string = str_new(empty_string);
	int dir = -1;
	void (*str_ccat_raw_p)(str_t*, const char*, size_t) = str_ccat_raw_fwd;
	size_t from = 0, l = get_len(sptr);

	if(pow < 0)
		dir = 1, str_ccat_raw_p = str_ccat_raw_rev, from = l-(l==0? 0: 1);

	while(pow != 0)
		pow += dir,
		str_ccat_raw_p(string, a_get(sptr, from), l);

	return string;
}

/*
Se toman los indices modulados a len+1, para que el comportamiento cumpla lo
establecido en mystring.h
*/
str_t* str_slice(str_t* sptr, size_t from, size_t to)
{
	str_t* string = str_new(empty_string);

	from %= get_len(sptr)+1, to %= get_len(sptr)+1;
	// printf(">slice %zu, %zu\n", from, to);

	if(from <= to)
		str_ccat_raw_fwd(string, a_get(sptr, from), to-from);
	else
		str_ccat_raw_rev(string, a_get(sptr, from-1), from-to);

	return string;
}

/*
Toma una lista de indices cuyo sentinela es un numero >= longitud del contenedor
y forma una nueva cadena a segun los caracteres que se encuentren en dichas
posiciones dentro del contenedor sptr
*/
str_t* str_seq1(str_t* sptr, size_t* idx_pos_list)
{
	str_t* string = str_new(empty_string);

	for(size_t l=get_len(sptr); *idx_pos_list<l; ++idx_pos_list)
		a_add(string, a_idx(sptr, *idx_pos_list))

	return string;
}

// Invierte una cadena mutandola
void str_rev(str_t* sptr)
{

	char *pi = a_get(sptr, 0), *pf = pi;
	for(size_t r=get_len(sptr), i=0, f=r-1; i!=r/2; ++i, --f)
	{
		char tmp = pi[i];
		pi[i] = pf[f], pf[f] = tmp;
	}
}

// Escribe una cadena a un stream, sirve para imprimirlas a stdout por ejemplo
size_t str_write(str_t* sptr, FILE* stream)
{
	return fwrite(arr(sptr), 1, get_len(sptr), stream);
}

/*
Intentara leer n bytes del stream, actualiza la longitud y espacio del sptr
segun los bytes leidos con exito, regresa lo que fread regresa
*/
size_t str_read(str_t* sptr, FILE* stream, size_t n)
{
	if(a_slots(sptr) < n)
		a_grow(sptr, n-a_slots(sptr))

	size_t rs = fread(arr(sptr)+get_len(sptr), 1, n, stream);
	get_len(sptr) += rs, a_slots(sptr) -= rs;

	return rs;
}

/*
Establece el arreglo de sptr como un bufer segun alguna bandera "mode" (stdio.h)
el bufer va desde el inicio de la cadena a bufsize-1, se maneja automaticamente
la memoria en caso de que no sea suficiente. Se asume que la cadena tendra
longitud bufsize, por lo que se establece la longitud de la cadena bufsize,
agregar contenidos a la cadena no corrompera el bufer a menos que realloc regrese
un apuntador a una posicion disinta a la que se establecio el bufer.

No tiene nada que ver con la practica, pero me resulto interesante hacer esto
*/
int str_buf(str_t* sptr, FILE* stream, size_t bufsize, int mode)
{
	if(a_slots(sptr) + get_len(sptr) < bufsize)
		a_grow(sptr, bufsize - a_slots(sptr))

	get_len(sptr) = bufsize, a_slots(sptr) = 0;

	return setvbuf(stream, arr(sptr), mode, bufsize);
}

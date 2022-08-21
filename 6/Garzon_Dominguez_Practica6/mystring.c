#include "mystring.h"

// Tipo completo para manejar cadenas
// Mantiene informacion sobre el uso de memoria ver macro en array.h
struct str_t
{
	ARRAY_TYPE(char)
	size_t hash_val;
};

// Prototipos para algunas funciones especiales usadas solo en este archivo
int my_strcmp_siz(const char *s1, const char *s2, size_t l1, size_t l2);
void* my_memmove_rev(void *dst, const void *src, size_t n);
size_t hash_nbytes(const char* str, size_t l);
char* substr_find(char* str, const char* sep);


// Envolver malloc y realloc en funciones que chequeen regreso nulo
void* my_malloc(size_t s);
void* my_realloc(void* p, size_t s);
void my_free(void* p);

const char *const nomem_err = 
"sin memoria disponible desde \"mystring.h\"... continuando...";

// Malloc que avisa a stderr si malloc falla, el programa continua
void* my_malloc(size_t s)
{
	// assert(s != 0);
	void* c = malloc_fun(s) ;
	if(c == NULL)
		fprintf(stderr, "%s\n", nomem_err);
	return c;
}

// Realloc que avisa a stderr si realloc falla, el programa continua
void* my_realloc(void* p, size_t s)
{
	// assert(s != 0);
	void* c = realloc_fun(p, s);
	if(c == NULL)
		fprintf(stderr, "%s\n", nomem_err),
		c = p;
	return c;
}

// free
void my_free(void* p)
{
	free_fun(p);
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
	if(s1 == s2)
		return 0;

	while( *s1 == *s2 && (*s1 & *s2) != '\0')
		++s1, ++s2;

	// printf("retval %d \"%s\" vs \"%s\"\n", *s1-*s2, s1, s2);
	return *s1 - *s2;
}

int my_memcmp(const void *s1, const void *s2, size_t n)
{
	const char *p1=(const char*)s1, *p2=(const char*)s2;

	size_t i=0;
	for(; i<n; ++i)
		if(p1[i] != p2[i])
			break;

	return n-i;
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

// Lo que hace memset de string,h
void* my_memset(void *b, int c, size_t len)
{
	char* bp = b;
	while(len-- != 0)
		*bp++ = c;
	return b;
}

// Apuntadores a funcion que usan los macros de array.h. No usar string.h
// asi que hacemos que apunten a las implementaciones de este archivo
void* (*memset_fun)(void *b, int c, size_t len) = my_memset;
void* (*memmove_fun)(void *dst, const void *src, size_t len) = my_memmove;

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


void str_ccat_raw_rev(str_t* sptr1, const char* str, size_t len);
void str_ccat_raw_fwd(str_t* sptr1, const char* str, size_t len);

// Compara dadas las cadenas s1, s2 y sus longitudes
// Regresa 0 si son iguales, si no, regresa otro numero
int my_strcmp_siz(const char *s1, const char *s2, size_t l1, size_t l2)
{
	if(s1 == s2)
		return 0;

	if(l1 == l2)
		return my_memcmp(s1, s2, l1);
	else
		l1 = 1;

	return l1;
}

// Copia n bytes de src en reversa y hacia adelante en dst
void* my_memmove_rev(void *dst, const void *src, size_t n)
{
	char *dstc = dst, *srcc = (char*)src;
	while(n-- != 0)
		*dstc++ = *srcc--;

	return dst;
}

// Concatena str a sptr1 usando una funcion memmove en particular
void str_ccat_raw_rev(str_t* sptr1, const char* str, size_t len)
{
	a_ccat_raw_with(sptr1, str, len, my_memmove_rev)
	sptr1->hash_val = hash_nbytes(arr(sptr1), get_len(sptr1));
}

// Concatena len bytes de str a sptr1 hacia enfrente
void str_ccat_raw_fwd(str_t* sptr1, const char* str, size_t len)
{
	a_ccat_raw(sptr1, str, len)
	sptr1->hash_val = hash_nbytes(arr(sptr1), get_len(sptr1));
}

// // Toma una referencia 
// str_t* str_new_ref(char* str, size_t strl)
// {
// 	str_t* string = my_malloc(sizeof *string);
// 	get_len(string) = strl;
// 	a_slots(string) = 0;
// 	arr(string) = str = realloc(str, strl);
// 	return string;
// }

// Crea una cadena con espacio de fitsiz bytes
str_t* str_new_fit(size_t fitsiz);

str_t* str_new_fit(size_t fitsiz)
{
	str_t* string = my_malloc(sizeof *string);
	a_init_siz(string, fitsiz);
	*arr(string) = 0;
	return string;
}

// Toma la longitud de str para copiar al contenedor str_t
str_t* str_new(const char* str)
{
	size_t strl = my_strlen(str);
	str_t* string = str_new_fit(strl);
	str_ccat_raw_fwd(string, str, strl);
	return string;
}

// Crea una nueva cadena con al menos n bytes de str
str_t* strn_new(const char* str, size_t n)
{
	str_t* string = str_new_fit(n);
	str_ccat_raw_fwd(string, str, n);
	return string;
}

// Copia una copia del contenedor str_t
str_t* str_new_cpy(str_t* sptr)
{
	str_t* string = str_new_fit(get_len(sptr));
	str_ccat(string, sptr);
	return string;
}

// Crea un contenedor con los contenidos de sptr1 y sptr2 en sequencia
str_t* str_new_ccat(str_t* sptr1, str_t* sptr2)
{
	str_t* string = str_new_fit(get_len(sptr1) + get_len(sptr2));
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
	str_t* string = str_new_fit(get_len(sptr));
	size_t from = get_len(sptr) - (get_len(sptr)==0? 0: 1);
	str_ccat_raw_rev(string, a_get(sptr, from), get_len(sptr));
	return string;
}

// Lee maximo max_size caracteres de stdin usando fgets, usa un bufer temporal
str_t* str_input(size_t max_size, const char* prompt)
{
	fprintf(stdout, "%s", prompt);
	++max_size;
	str_t* in = NULL;

	char* buf = my_malloc(max_size);

	if(fgets(buf, max_size, stdin) != NULL)
		in = strn_new(buf, my_strlen(buf)-1);

	my_free(buf);

	return in;
}

const char* str_arr(str_t* sptr)
{
	return arr(sptr);
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
	sptr1->hash_val = hash_nbytes(arr(sptr1), get_len(sptr1));
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

/*
Funcion de utilidad para str_sep, toma una cadena str y sep, ambas terminadas
en nul. Regresa un apuntador al inicio de la secuencia encontrada dentro de str,
si no se encuentra la secuencia, regresa NULL.
Como caso especial si seq es la cadena vacia, se regresara NULL, ya que en teoria
hay infinitas cadenas vacias concatenadas dentro de cualquier cadena.
*/
char* substr_find(char* str, const char* seq)
{
	const char *save_sep = seq, nul = '\0';
	char* save_str = NULL;

	if(*seq == nul)
		return save_str;

	while(1)
	{
		seq = save_sep;
		while(*str == *seq && *str != nul && *seq != nul)
			++str, ++seq;

		if(*seq == nul)
		{
			save_str = str - (seq-save_sep);
			break;
		}

		if(*str != nul)
			++str;
		else
			break;
	}

	return save_str;
}

/*
Separa una cadena str cada que se encuentre la secuencia sep dentro de str.
str es sobreescrita con un '\0' al inicio de cada secuencia sep que se encuentre
si sep es la cadena vacia o una secuencia que no se encuentra en str, entonces,
se regresa una lista que solo contiene a str sin modificaciones.
En otro caso se devuelve una lista con apuntadores a los tokens generados.
La lista incluye un sentinela NULL al final de esta.
Se asume que las cadenas str y seq estan terminadas en nul.
*/
array_t* str_sep(char* str, const char* sep)
{
	array_t* string_list = array_new(1);
	size_t strl = my_strlen(str), sepl = my_strlen(sep);

	const char nul = '\0', *end_str = str+strl;
	char* t = NULL;

	while(t = substr_find(str, sep), str <= end_str)
	{
		if(str != t)
			array_add(string_list, str);

		if(t != NULL)
			*t = nul, str = t + sepl;
		else 
			break;
	}

	size_t last = get_size(string_list)-1;
	if(my_strlen(get_item(string_list, last)) != 0)
		last += 1;
	set_item(string_list, last, NULL);

	return string_list;
}

/*
			// offset (t-str) + sepl

		// if(t-str != 0)
		// {
		// 	printf("adding '%s' %p - %p = %tu\n", str, t, str, t-str);
		// 	array_add(string_list, str);
		// }
	while(t != NULL)
	{
		t = substr_find(str, sep);
		if(t != NULL)
		{
			*t = 0;

			if(t-str != 0)
				array_add(string_list, str);

			str += (t-str) + sepl;
		}
	}


*/ 

/*
Funcion hash_nbytes, Inspirada en la funcion:
https://en.wikipedia.org/wiki/Jenkins_hash_function
Los numeros magicos 7 y 9 son los que tras experimentacion, encontre que
daban por lo general menor numero de colisiones para la tabla
*/
size_t hash_nbytes(const char* str, size_t l) 
{
	l = avoid_fpe(l);
	size_t h = 0;
	for(size_t i=0; i<l; ++i)
	{
		h += str[i%l];
		h += h >> 10;
		h ^= h << 6;
	}
	h += h << 3, h ^= h >> 11, h ^= h << 15;

	return h;
}

size_t str_hash(str_t* obj)
{
	if(obj->hash_val == 0 && get_len(obj) != 0)
		obj->hash_val = hash_nbytes(arr(obj), get_len(obj));
	return obj->hash_val;
}

/*
* Tipos para la implementacion de conjuntos
* Estan implementados como tablas hash abiertas. La funcion Hash no es muy 
* "perfecta", asi que las tablas estan diseñadas para aumentar el tamaño
* de las cubetas si es que despues rehashear la tabla hay mas de cierta
* cantidad de elementos que colisionan (set->max_per_bucket)

* Podemos visualizar como estan organizados en memoria los conjuntos, vease
* los siguientes 3 tipos para mejor entendimiento:

{strset_t}
|[bucket_t]	| -> [strnode] -> [strnode] -> .... [strnode]
|			|		v 			v 					v
|			|	[str_t]		[str_t]				[str_t]
|			|
|[bucket_t]	| -> [strnode] -> [strnode] -> .... [strnode]
|			|		v 			v 					v
|			|	[str_t]		[str_t]				[str_t]
|			|	
|[bucket_t] |-> [strnode] -> [strnode] -> .... [strnode]
|			|		v 			v 					v
|			|	[str_t]		[str_t]				[str_t]
|			|
|[bucket_t] |-> [strnode] -> [strnode] -> .... [strnode]
|			|		v 			v 					v
|			|	[str_t]		[str_t]				[str_t]
|			|
|...		|
|			|
|[bucket_t] |-> [strnode] -> [strnode] -> .... [strnode]
					v 			v 					v
				[str_t]		[str_t]				[str_t]


* Las cubetas son una lista simple circular con dummy, vease las estructuras
* de datos en data_structures/singly.h (parte de un pequeño proyecto mio cuyo 
* proposito es preocuparse menos por la implementacion de una lista y que estas 
* sean generales para todo tipo de dato).

Las listas estan organizadas en memoria basicamente como:

{contenedor: longitud, [tipo: dummy]}

...my]->[nodo]->[nodo]->[nodo]->[dum...
Tienen macros para iterar facilmente en loops, para remover y agregar
elementos, ademas, es facil usar iteradores del mismo tipo contenido en la lista
lo cual permite rapidamente obtener atributos del tipo contenido, com op flecha
*/

typedef struct strnode_t
{
	SINGLY_ND(struct strnode_t);
	str_t* obj;
}strnode_t;

typedef struct
{
	SINGLY_TYPE(strnode_t)
}bucket_t;

struct strset_t
{
	size_t size;
	unsigned int max_per_bucket;
	ARRAY_TYPE(bucket_t*)
};

// Busca el elemento en la cubeta designada
int element_repeated(bucket_t* bucket, const char* elem, size_t l);

// Busca un elemento dentro de un conjunto regresa 0 si no se encuentra
int element_repeated_set(strset_t* set, str_t* sptr);


/*
Busca un elemento dentro de un conjunto regresa el elemento que pertenece al
conjunto si lo encuentra, si no, regresa NULL
*/
str_t* element_repeated_set_ret_ref(strset_t* set, str_t* sptr);

// Crea un nueva cubeta
bucket_t* bucket(void);

// Agrega sptr a una lista bucket
void bucket_add(bucket_t* bucket, str_t* sptr);

// Inicializa las listas buckets dentro del arreglo del set en el rango from, to
void init_buckets(strset_t* set, size_t from, size_t to);

// Remapea todos los elementos de las cubetas del set segun el nuevo tamaño
void rehash_table(strset_t* set);

// Agregar directamente sptr a una tabla dada la cubeta destino
void table_add(strset_t* set, bucket_t* bucket, str_t* sptr);

// Agrega el objeto sptr a un conjunto sin checar por repeticiones
void set_add__(strset_t* set, str_t* sptr);

// Agrega el objeto sptr a un conjunto checando por repeticiones
void set_add(strset_t* set, str_t* sptr);

// LO mismo que Cartesian, pero sirve para potencias negativas en Power()
strset_t* Cartesian_rv(strset_t* set1, strset_t* set2);

// Constructores y operaciones fundamentales para las cubetas de la tabla
bucket_t* bucket(void)
{
	bucket_t* buck = my_malloc(sizeof *buck);
	sl_init(buck)
	return buck;
}

void bucket_add(bucket_t* bucket, str_t* sptr)
{
	strnode_t* ins = my_malloc(sizeof *ins);
	ins->obj = sptr;
	sl_ins(bucket, &get_head(bucket), ins)
}

void init_buckets(strset_t* set, size_t from, size_t to)
{
	for(; from<to; ++from)
		arr(set)[from] = bucket();
}

// Constructor para un objeto conjunto
strset_t* set_new(void)
{
	enum magic_nums {set_initial_siz_ = 12, set_max_per_bucket_ = 10,};

	strset_t* set = my_malloc(sizeof *set);
	a_init_siz(set, set_initial_siz_)
	init_buckets(set, 0, a_slots(set));

	set->max_per_bucket = set_max_per_bucket_;

	return set;
}

// Crece el conjunto al doble de capacidad actual y remapea todos los
// elementos (si es que mapea a otro indice de cubeta) de las cubetas
void rehash_table(strset_t* set)
{
	// printf("rehashing!\n");
	size_t n=a_slots(set);
	a_grow(set, n)
	init_buckets(set, n, a_slots(set));

	n = a_slots(set);
	size_t i=0, max=set->max_per_bucket;
	for( ; i<n; ++i )
	{
		bucket_t* b = arr(set)[i];
		for( strnode_t** j=&get_head(b); iauto(b, *j); )
		{
			str_t* sptr = (*j)->obj;
			size_t hash = str_hash(sptr), k = hash%n;

			if(k != i)
			{
				bucket_t* dst = arr(set)[k];
				strnode_t* save = *j;
				sl_pop(b, j)
				sl_ins(dst, &get_head(dst), save)
			}
			else
				j=&get_next(*j);
		}
		if(get_len(b) > max)
			max = get_len(b);
	}

	// printf("remaped %zu, unremaped %zu\n", rm, urm);

	if(max > set->max_per_bucket)
		set->max_per_bucket = max*2;
}

// Agrega sptr a una cubeta, si se llega a la capacidad maxima de la cubeta
// Entonces se rehashea toda la tabla
void table_add(strset_t* set, bucket_t* bucket, str_t* sptr)
{
	++set->size;
	bucket_add(bucket, sptr);
	if(get_len(bucket) > set->max_per_bucket)
		rehash_table(set);
}

// Busca un elemento elem dentro de una cubeta regresa 0 si no se encuentra
int element_repeated(bucket_t* bucket, const char* elem, size_t l)
{
	int buck_repeated = 0;

	for( strnode_t* i=get_head(bucket); iauto(bucket, i); mv_next(i) )
		if(my_strcmp_siz(arr(i->obj), elem, get_len(i->obj), l) == 0)
		{
			buck_repeated = 1;
			break;
		}

	return buck_repeated;
}

// Busca un elemento dentro de un conjunto regresa 0 si no se encuentra
int element_repeated_set(strset_t* set, str_t* sptr)
{
	int buck_repeated = 0;

	const char* str = arr(sptr);
	size_t l=get_len(sptr), p=str_hash(sptr)%a_slots(set);
	bucket_t* bucket = arr(set)[p];

	for( strnode_t* i=get_head(bucket); iauto(bucket, i); mv_next(i) )
		if(my_strcmp_siz(arr(i->obj), str, get_len(i->obj), l) == 0)
		{
			buck_repeated = 1;
			break;
		}
	
	return buck_repeated;
}

str_t* element_repeated_set_ret_ref(strset_t* set, str_t* sptr)
{
	str_t* elem = NULL;

	const char* str = arr(sptr);
	size_t l=get_len(sptr), p=str_hash(sptr)%a_slots(set);
	bucket_t* bucket = arr(set)[p];

	for( strnode_t* i=get_head(bucket); iauto(bucket, i); mv_next(i) )
		if(my_strcmp_siz(arr(i->obj), str, get_len(i->obj), l) == 0)
		{
			elem = i->obj;
			break;
		}
	
	return elem;
}


// Agrega el objeto sptr a un conjunto sin checar por repeticiones
void set_add__(strset_t* set, str_t* sptr)
{
	size_t p = str_hash(sptr)%a_slots(set);
	bucket_t* bucket = arr(set)[p];
	table_add(set, bucket, sptr);
}

// Agrega un objeto sptr al conjunto checando por repeticiones
void set_add(strset_t* set, str_t* sptr)
{
	char* str = arr(sptr);
	size_t l = get_len(sptr), p = str_hash(sptr)%a_slots(set);
	bucket_t* bucket = arr(set)[p];
	if(!element_repeated(bucket, str, l) )
		table_add(set, bucket, sptr);
}

// Crea un nuevo conjunto desde un arreglo terminado con un sentinela null
strset_t* set_from_arr(const char *array[])
{
	strset_t* set = set_new();
	while(*array != NULL)
		set_add_raw(set, *array++);
	return set;
}

// Agrega un objeto cadena a creado a partir de str, si y solo si
// una cadena con los contenidos de str no existe aun en set
void set_add_raw(strset_t* set, const char* str)
{
	size_t l=my_strlen(str), p=hash_nbytes(str, l)%a_slots(set);
	bucket_t* bucket = arr(set)[p];
	// printf("%p st %zu mod %zu = %zu, '%s'\n", set, Hash(str, l), 
	// 	a_slots(set), p, str);

	if(!element_repeated( bucket, str, l ))
		table_add(set, bucket, strn_new(str, l));
}

void set_add_ref(strset_t* set, str_t* obj)
{
	set_add(set, obj);
}

// Union de dos conjuntos, vease que los objetos cadena son solo un apuntador
// al elemento original, se ahorra memoria
strset_t* Union(strset_t* set1, strset_t* set2)
{
	strset_t* uset = set_new();

	for(size_t i=0, n=a_slots(set1); i<n; ++i)
	{
		bucket_t* bucket = arr(set1)[i];
		for( strnode_t* j=get_head(bucket); iauto(bucket, j); mv_next(j) )
			set_add__(uset, j->obj);
	}

	for(size_t i=0, n=a_slots(set2); i<n; ++i)
	{
		bucket_t* bucket = arr(set2)[i];
		for( strnode_t* j=get_head(bucket); iauto(bucket, j); mv_next(j) )
			set_add(uset, j->obj);
	}

	return uset;
}

// Interseccion entre set1 y set2, los objetos agregados son referencias
strset_t* Intersection(strset_t* set1, strset_t* set2)
{
	strset_t* iset = set_new();

	for(size_t i=0, n=a_slots(set2); i<n; ++i)
	{
		bucket_t* bucket = arr(set2)[i];
		for( strnode_t* j=get_head(bucket); iauto(bucket, j); mv_next(j) )
		{
			if(element_repeated_set(set1, j->obj))
				set_add__(iset, j->obj);
		}
	}

	return iset;	
}

// Diferencia entre set1 y set2, los objetos agregados son referencias
strset_t* Difference(strset_t* set1, strset_t* set2)
{
	strset_t* dset = set_new();

	for(size_t i=0, n=a_slots(set1); i<n; ++i)
	{
		bucket_t* bucket = arr(set1)[i];
		for( strnode_t* j=get_head(bucket); iauto(bucket, j); mv_next(j) )
		{
			if(!element_repeated_set(set2, j->obj))
				set_add__(dset, j->obj);
		}
	}

	return dset;
}

// Busca si el elemento obj existe en el set (segun los contenidos de obj)
str_t* Element_ref(strset_t* set1, str_t* obj)
{
	return element_repeated_set_ret_ref(set1, obj);
}

// Busca el elemento elem y dada su longitud dentro del conjunto,
// Regresa un apuntador al elemento si se encuentra, NULL de lo contrario
str_t* Element(strset_t* set, const char* elem, size_t l)
{
	size_t h = hash_nbytes(elem, l)%a_slots(set);
	bucket_t* bucket = arr(set)[h];
	str_t* retval = NULL;

	if(get_len(bucket) == 1)
		return get_head(bucket)->obj;

	for( strnode_t* i=get_head(bucket); iauto(bucket, i); mv_next(i) )
		if(my_strcmp_siz(arr(i->obj), elem, get_len(i->obj), l) == 0)
		{
			retval = i->obj;
			break;
		}

	return retval;
}

// Producto cartesiano entre el conjunto set1 y set2: concatenacion de lenguajes
// Los objetos agregados son objetos cadena nuevos
strset_t* Cartesian(strset_t* set1, strset_t* set2)
{
	strset_t* cset = set_new();

	str_t* cmpbuf = NULL;
	for(size_t i=0, n=a_slots(set1); i<n; ++i)
	{
		bucket_t* b1 = arr(set1)[i];

		for( strnode_t* j=get_head(b1); iauto(b1, j); mv_next(j) )
		{
			for(size_t c=0, k=a_slots(set2); c<k; ++c)
			{
				bucket_t* b2 = arr(set2)[c];
				for( strnode_t* a=get_head(b2); iauto(b2, a); mv_next(a) )
				{
					cmpbuf = str_new_ccat(j->obj, a->obj);

					if(!element_repeated_set(cset, cmpbuf))
						set_add__(cset, cmpbuf);
					else
						str_del(cmpbuf);
				}
			}
		}
	}

	return cset;
}

// Lo mismo que Cartesian, pero sirve para potencias negativas en Power()
strset_t* Cartesian_rv(strset_t* set1, strset_t* set2)
{
	strset_t* cset = set_new();

	str_t* cmpbuf = NULL;
	for(size_t i=0, n=a_slots(set1); i<n; ++i)
	{
		bucket_t* b1 = arr(set1)[i];

		for( strnode_t* j=get_head(b1); iauto(b1, j); mv_next(j) )
		{
			for(size_t c=0, k=a_slots(set2); c<k; ++c)
			{
				bucket_t* b2 = arr(set2)[c];
				for( strnode_t* a=get_head(b2); iauto(b2, a); mv_next(a) )
				{
					cmpbuf = str_new_cpy(j->obj);
					str_ccat_raw_rev(cmpbuf, arr(a->obj)+get_len(a->obj)-1, 
						get_len(a->obj));

					if(!element_repeated_set(cset, cmpbuf))
						set_add__(cset, cmpbuf);
					else
						str_del(cmpbuf);
				}
			}
		}
	}

	return cset;
}


// Potencia de un conjunto (concatenacion consigo mismo pow veces)
// Concatenacion inversa para potencias negativas
strset_t* Power(strset_t* set1, long pow)
{
	strset_t *pset = set_new(), *w = pset;
	if(pow >= 0)
	{
		set_add_raw(pset, empty_string);
		while(pow-- != 0)
		{
			pset = Cartesian(pset, set1);
			set_strong_del(w);
			w = pset;
		}
	}

	else if(pow < 0)
	{
		set_add_raw(pset, empty_string);
		while(pow++ != 0)
		{
			pset = Cartesian_rv(pset, set1);
			set_strong_del(w);
			w = pset;
		}
	}


	return pset;
}


/*
Computa el conjunto definido como la union de set1 a la 0 hasta set1 a la lim.
El "truco" aqui, es que se concatena set1^n con set1 U {e}; e = "", |e| = 0 .

La operacion que se hace es entonces:

set1^n x (set1 U {e}) = (set1^n)x(set1^n) U (set1^n)x({e}) = set1^n+1 U set1^n
(por que el producto cartesiano es distributivo sobre la union)

Esto nos ahorra el tener que guardar cada potencia y despues reiterar para obtener
la cerradura de kleene.
*/
strset_t* Kleene_star(strset_t* set1, long lim)
{
	strset_t *pset = set_new(), *w = pset, *trick = NULL;
	
	set_add_raw(pset, empty_string);
	trick = Cartesian(set1, pset);
	set_add_raw(trick, empty_string);

	while(lim-- != 0)
	{
		pset = Cartesian(pset, trick);
		set_strong_del(w);
		w = pset;
	}

	set_strong_del(trick);

	return pset;
}

/*
Computa el conjunto definido como la union de set1 a la 1 hasta set1 a la lim
lim = 0 no esta definido para kleene+, si se le pasa lim = 0, la funcion
computara kleene+ desde 1 hasta long_max.

El caso base se computa fuera del ciclo, por facilidad, y en general, porque
Kleene+ esta definido para numeros >= 1
*/
strset_t* Kleene_plus(strset_t* set1, long lim)
{
	strset_t *pset = set_new(), *w = pset, *trick = NULL;
	
	set_add_raw(pset, empty_string);
	trick = Cartesian(set1, pset);
	pset = Cartesian(set1, pset);
	set_add_raw(trick, empty_string);

	set_strong_del(w);
	w = pset;

	while(--lim != 0)
	{
		pset = Cartesian(pset, trick);
		set_strong_del(w);
		w = pset;
	}

	set_strong_del(trick);

	return pset;
}

// Regresa el numero de elementos de un conjunto
size_t Count(strset_t* set)
{
	return set->size;
}

/*
A lo largo del documento nos hemos referido a un tipo de objetos cadena como
copia sencilla, esta funcion libera conjuntos que tengan tal tipo de copia
dentro de sus cubetas, (asumiendo que el llamador sabe que el conjunto en
efecto contiene solo copias debiles de objetos cadena, o referenciados por
mas de un apuntador)
*/
void set_weak_del(strset_t* set)
{
	for(size_t i=0, n=a_slots(set); i<n; ++i)
	{
		bucket_t* b = arr(set)[i];
		for( strnode_t** j=&get_head(b); !lempty(b); )
		{
			strnode_t* to_free = *j;
			sl_pop(b, j)
			my_free(to_free);
		}
		my_free(b);
	}
	a_del(set)
	my_free(set);
}

/*
A lo largo del documento nos hemos referido a un tipo de objetos cadena como
copia completa, esta funcion libera conjuntos que tengan tal tipo de copia
dentro de sus cubetas, (asumiendo que el llamador sabe que estos objetos,
no estan refenciados por mas de un apuntador o no se necesitaran mas)
*/
void set_strong_del(strset_t* set)
{
	for(size_t i=0, n=a_slots(set); i<n; ++i)
	{
		bucket_t* b = arr(set)[i];
		for( strnode_t** j=&get_head(b); !lempty(b); )
		{
			strnode_t* to_free = *j;
			sl_pop(b, j)
			str_del(to_free->obj), 
			my_free(to_free);
		}
		my_free(b);
	}
	a_del(set)
	my_free(set);
}

// Escribe un conjunto de cadenas sin formato a un stream
size_t set_write(strset_t* set, FILE* stream)
{
	size_t succ_write = 0;

	for( size_t i=0, n=a_slots(set); i<n; ++i )
	{
		bucket_t* b = arr(set)[i];
		for( strnode_t* i=get_head(b); iauto(b, i); mv_next(i) )
			str_write(i->obj, stream);
	}

	return succ_write;
}

// Imprime un conjunto de cadenas con formato a un stream
size_t set_writef(strset_t* set, FILE* stream, const char* title)
{
	size_t succ_write = 0;

	succ_write += fprintf(stream, "%s{", title);

	for( size_t i=0, n=a_slots(set); i<n; ++i )
	{
		bucket_t* b = arr(set)[i];

		for( strnode_t* j=get_head(b); iauto(b, j); mv_next(j) )
		{
			succ_write += fprintf(stream, "\"");
			succ_write += str_write(j->obj, stream);
			succ_write += fprintf(stream, "\"");
			succ_write += fprintf(stream, ", ");
		}
	}

	succ_write += fprintf(stream, "}\n");

	return succ_write;
}

/*
Estas funciones son para dar detalles sobre como esta funcionando un
Objeto conjunto internamente: tamaño del arreglo de cubetas, cubetas
ocupadas, y cubetas no coupadas
*/
size_t set_writedbg(strset_t* set)
{
	size_t succ_write = 0, elems=0;

	for( size_t i=0, n=a_slots(set); i<n; ++i )
	{
		bucket_t* b = arr(set)[i];

		if(get_len(b) > 0)
		{
			succ_write++;
			elems += get_len(b);
		}
			// printf("Bucket id [%zu] %zu elements\n", i, get_len(b));

		// for( strnode_t* j=get_head(b); iauto(b, j); mv_next(j) )
		// 	succ_write += printf("'%s[%zu]', ", arr(j->obj), get_len(j->obj));

	}
	// printf("\n");
	printf("Elements %zu, Buckets %zu/%zu\n", set_impl_total_elems(set), 
		succ_write, a_slots(set));
	return succ_write;
}

size_t set_impl_total_elems(strset_t* set)
{
	size_t total = 0;
	for(size_t i=0, n=a_slots(set); i<n; ++i)
	{
		bucket_t* b = arr(set)[i];
		for(strnode_t* j=get_head(b); iauto(b, j); mv_next(j))
			total++;
	}

	return total;
}

size_t set_impl_total_buckets(strset_t* set)
{
	return a_slots(set);
}

size_t set_impl_used_buckets(strset_t* set)
{
	size_t count = 0;
	for( size_t i=0, n=a_slots(set); i<n; ++i )
	{
		bucket_t* b = arr(set)[i];
		count += get_len(b) != 0 ? 1: 0;
	}
	return count;
}

size_t set_impl_free_buckets(strset_t* set)
{
	return set_impl_total_buckets(set) - set_impl_used_buckets(set);
}


// Alternativa para Kleene*
/*strset_t* Kleene_star(strset_t* set1, long lim)
{
	strset_t *pset = set_new(), *kl = NULL, *kl_a[lim+1], *klfree = NULL;
	set_add_raw(pset, empty_string);

	klfree = kl = kl_a[lim] = pset;
	long pw = lim;
	while(pw-- != 0)
	{
		kl_a[pw] = pset = Cartesian(pset, set1);
		kl = Union(kl_a[pw], kl);
		set_weak_del(klfree);
		klfree = kl;
	}

	for(pw=0; pw<lim; ++pw)
		set_weak_del(kl_a[pw]);

	return kl;
}*/

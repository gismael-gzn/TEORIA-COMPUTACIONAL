#include "memutils.h"

// Definiciones de simbolos externos desde "data_structures/container.h"
void* (*malloc_fun)(size_t size) = malloc;
void* (*realloc_fun)(void *b, size_t bytes) = realloc;
void (*free_fun)(void *ptr) = free;

// Definiciones de simbolos externos de este modulo
array_t* utils_memory = NULL;
size_t utils_allocs = 0, utils_reallocs = 0, utils_frees = 0;

const char *errors[] = 
{
	"malloc(%zu) regresa NULL... Continuando\n",
	"realloc(%p, %zu) regresa NULL... Continuando\n",
	"Esta direccion <%p> no apunta a ningun bloque en la lista utils_memory\n",
};

// Envoltorios para malloc, realloc, free
void* utils_malloc(size_t siz);
void* utiils_realloc(void* p, size_t siz);
void utils_free(void* p);

/*
Rutina de limpieza, libera los contenidos del arreglo utils_memory, y tambien
libera el arreglo utils_memory. Si se usa en codigo, asegurese de utilizar,
*/
void mem_utils_clean(void);

/*
Inicializa el modulo de utilidades de memoria
*Si se llama con la bandera free_at_exit, cambia los apuntadores a funcion
declarados en "data_structures/container.h" a las funciones proveidas aqui
que mantienen trazo de allocs reallocs y frees. cuando el programa sale
(mediante exit o llega al final de main) se liberaran los bloques que no hayan
sido explicitamente liberados antes.

*Si se llama con la bandera never_free, entonces los apuntadores a funcion
apuntaran a las funciones estandar malloc, realloc y free. Y no se ejecutara
ninguna rutina de limpieza cuando el programa termine.
*/
void mem_utils(enum mem_utils_flags flag)
{
	utils_memory = array_new(64);

	utils_allocs = utils_reallocs = utils_frees = 0;

	if(flag == free_at_exit)
		atexit(mem_utils_clean);

	malloc_fun = utils_malloc;
	realloc_fun = utiils_realloc;
	free_fun = utils_free;
}

/*
Libera los apuntadores restantes en el arreglo utils_memory, solo se ejecuta
cuando el programa sale por alguna razon; como la funcion exit, llegar al final
de main, y si la bandera free_at_exit se establecio al llamar mem_utils.
*/
void mem_utils_clean(void)
{
/*	fprintf(stderr, "%zu utils_allocs\n%zu utils_reallocs\n%zu utils_frees\n", 
		utils_allocs, utils_reallocs, utils_frees);
	fprintf(stderr, "%zu on utils_memory\n", get_size(utils_memory));
*/
	for(size_t i=get_size(utils_memory); i; --i)
		free(get_item(utils_memory, i));

	free((void*)get_memblock(utils_memory));
	free(utils_memory);	
}

/*
Nueva alojacion, usando stdlib malloc, agrega el apuntador devuelto al arreglo
utils_memory, si es NULL, se imprime un error y se continua sin salir.
Ademas, si se solicita un alloc de 0, regresa un alloc de 1 byte.
*/
void* utils_malloc(size_t siz)
{
	++utils_allocs;

	if(siz == 0)
		siz = 1;

	void* m = malloc(siz);

	if(m == NULL)
		fprintf(stderr, errors[0], siz);

	realloc_fun = realloc;
	array_add(utils_memory, m);
	realloc_fun = utiils_realloc;

	return m;
}

/*
Realojacion, usando stdlib realloc. Busca el apuntador pasado dentro del arreglo
para poder sustituirlo con lo que sea que regrese realloc.
Si realloc regresa NULL, se imprime un error y se continua el programa.
*/
void* utiils_realloc(void* p, size_t siz)
{
	++utils_reallocs;
	void* rv = NULL;

	void** elemp = find_item(utils_memory, p, 0, NULL);
	if(elemp != NULL)
	{
		rv = realloc(p, siz);

		realloc_fun = realloc;
		set_item(utils_memory, get_index(utils_memory, elemp), rv);
		realloc_fun = utiils_realloc;
	}
	else
		fprintf(stderr, errors[1], p, siz);

	return rv;
}

/*
Busca el apuntador pasado dentro del arreglo utils_memory, si lo encuentra
lo libera, si no, imprime un error y se continua con la ejecucion del programa
*/
void utils_free(void* p)
{
	++utils_frees;

	void** elemp = find_item(utils_memory, p, 0, NULL);
	if(elemp != NULL)
		set_item(utils_memory, get_index(utils_memory, elemp), NULL);
	else
		fprintf(stderr, errors[1], p);

	free(p);
}

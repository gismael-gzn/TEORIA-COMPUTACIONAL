#include "automata.h"
#include "memutils.h"

void exit_with_msg(const char* msg);
void instruct(void);

void print_state(void* s);
size_t key_cmp(void* m1, void* m2);
size_t state_cmp(void* s1, void* s2);

str_t* open_and_load_automaton(int argc, const char* argv[]);
void parse_and_create_context(char* a_as_chr);

array_t* dynamic_delta(void* state_obj, const char* symbol);
void step(str_t* expr, size_t idx, void* obj);

/*
Estructura que representa un mapeo para la funcion delta.
Los primeros 2 elementos se encuentran en una estructura anonima para simbolizar
que son la "llave" para mapear a un resultado, asi, se puede guardar un par
(estado, simbolo), y su resultado dentro de un mismo arreglo.
En teoria la estructura que se describe es un mapa. Pero estos no suelen ser
implementados como listas lineales. Asi que preferi este metodo.
En cuestion, la estructura representa una regla de asociacion, de la forma:
(estado, simbolo)->resultado o una 3-tupla (estado, simbolo, resultado)
*/
typedef struct
{
	struct { const char *st, *sy; }; //key
	array_t* result;
}mapping_t;

// Contexto del programa en ejecucion
struct context
{
	automata_t* automaton;
	array_t
	*delta_mapping, // array of mapping_t*
	*path, // array of char*
	*empty_set; // array void*
	str_t* expr;
}context;


// main
int main(int argc, char const *argv[])
{
	mem_utils(free_at_exit);

	str_t* automaton_as_str = open_and_load_automaton(argc, argv);
	char* automaton_as_chr = (char*)str_arr(automaton_as_str);
	parse_and_create_context(automaton_as_chr);

	instruct();
	str_t* seq[] = { str_new("exit"), str_new("load"), };
	while(1)
	{
		context.expr = str_input(127, ">> ");

		if(str_hash(context.expr) == str_hash(seq[0]))
			break;
		else if(str_hash(context.expr) == str_hash(seq[1]))
		{
			printf("Leyendo automata...\n\n");
			automaton_as_str = open_and_load_automaton(argc, argv);
			automaton_as_chr = (char*)str_arr(automaton_as_str);
			parse_and_create_context(automaton_as_chr);
			continue;
		}

		traverse_all_paths(context.automaton, context.expr);

		context.path = NULL;
	}

	return 0;
}


/*
Abre un archivo con la ruta especificada en el segundo argumento con el que
se inicio el programa. checa que la cantidad de arumentos sea valida y tambien
checa si el archivo se abrio con exito. De lo contrario se sale con un mensaje
de error. Posteriormente carga los contenidos del archivo en un objeto cadena.
Los contenidos del objeto cadena se fuerzan a terminal en nul para facilitar el
analisis de los contenidos del archivo.
*/
str_t* open_and_load_automaton(int argc, const char* argv[])
{
	FILE* aut = NULL;
	if(argc != 2)
		exit_with_msg("uso: ./automata <nombre del archivo>");

	aut = fopen(argv[1], "r");
	if(aut == NULL)
	{
		char buf[4096] = {0};
		snprintf(buf, 4096, "No se pudo abrir el archivo \"%s\"", argv[1]);
		exit_with_msg(buf);
	}

	str_t* txt = str_new(empty_string);
	while(str_read(txt, aut, 128) != 0);
	fclose(aut);

	char* force_nul_end = (char*)str_arr(txt)+str_len(txt);
	*force_nul_end = '\0';

	return txt;
}

/*
El trabajo "pesado" se encuentra aqui, recibe los contenidos del archivo txt
como un solo arreglo de caracteres nul terminado. 
Analiza el arreglo y carga dicha informacion en el contexto del programa.
*Para esta explicacion los apuntadores a char seran sinonimo de cadena.
*La palabra parte se refiere a separar una cadena e introducir las subcadenas
	generadas en un arreglo dinamico terminado con sentinela NULL
En general su funcionamiento es:
1. Parte la cadena total en subcadenas cada que encuentra una nueva linea
2. Las primeras 4 lineas representan los primeros 4 elementos de la 5-tupla
3. Desde la linea 5 hasta la ultima linea, se considera la definicion de la
	funcion delta. Cada de estas lineas se parte cada ",".
	Si las subcadenas por cada linea son 2, se considera que esa regla
	asocia a un conjunto vacio (estado, simbolo, {}), si 3 o mas elementos,
	se considera que esa regla asocia al tercer elemento (se ignora el resto de
	elementos en ese renglon)
	Note que los primeros 2 elementos de la tupla son la llave de un mapping_t
	el tercero es el valor asociado a la llave.
*/
void parse_and_create_context(char* a_as_chr)
{
	// Separa el texto en lineas
	array_t* lines = str_sep(a_as_chr, "\r\n");
	if(get_size(lines)-1 == 1)
	{
		// printf("... Intentando con nuevas lineas *nix\n");
		array_del(lines);
		lines = str_sep(a_as_chr, "\n");
	}

	automata_t* aut = NULL;
	const char* sep = ",";

	// Los primeros 4 renglones del archivo
	enum part {states=0, alphab, beg_st, end_st, tup_sz};
	array_t* tup[tup_sz] = {0}; 
	for(enum part i=states; i<tup_sz; ++i)
	{
		char* tuple_part = get_item(lines, i);
		tup[i] = str_sep(tuple_part, sep);
	}
	strset_t* sigma = set_from_arr((const char**)get_memblock(tup[alphab]));

	aut = Automata(states[tup], sigma, dynamic_delta, get_item(beg_st[tup], 0), 
		end_st[tup], step);

	/*
	El resto de n renglones se considera la funcion delta.
	Se crean n arreglos de 3 elementos donde cada elemento la 3-tupla: 
	(estado, simbolo, resultado) con tipos (char*, char*, char*)
	*/
	array_t* map[get_size(lines)-tup_sz]; 	// [] of array of (char*)
	for(size_t i=tup_sz, n=get_size(lines)-1; i<n; ++i)
	{
		char* tuple_part = get_item(lines, i);
		if(tuple_part == NULL)
			break;
		map[i-tup_sz] = str_sep(tuple_part, sep);
		// print_array(map[i-tup_sz], 1);
	}

	/*
	Del arreglo de 3-tuplas, se genera un arreglo que representa la funcion
	delta mediante una estructura que representa una 3-tupla:
	(estado, simbolo, resultado) con tipos (char*, char*, array_t*).

	Para tuplas cuyos primeros 2 elementos se repitan, se agregara el resultado
	del mapeo al 3 elemento de la tupla, ya que este representa un conjunto
	o las opciones que tiene el automata al recorrer el camino.
	(o sea, no hay estado, simbolo duplicados DENTRO de delta_mapping)
	*/
	array_t* delta = array_new(1); 	// array of (mapping_t*)
	for(size_t i=tup_sz, n=get_size(lines)-1; i<n; ++i)
	{
		size_t tup_siz = get_size(map[i-tup_sz])-1;

		mapping_t key = 
		{{get_item(map[i-tup_sz], 0), get_item(map[i-tup_sz], 1)}, NULL};

/*		printf("adding (%s, %s)->%s\n", get_item(map[i-tup_sz], 0), 
			get_item(map[i-tup_sz], 1), get_item(map[i-tup_sz], 2));
*/
		void** pos = find_item(delta, &key, 0, key_cmp);
		if(pos == NULL)
		{
			mapping_t* tup3 = malloc_fun(sizeof *tup3);
			*tup3 = key;
			tup3->result = array_new(1);
			if(tup_siz == 3)
				array_add(tup3->result, get_item(map[i-tup_sz], 2));
			array_add(delta, tup3);
		}
		else
		{
			mapping_t* cm = *pos;
			array_add(cm->result, get_item(map[i-tup_sz], 2));
		}
	}

	context = (struct context){ aut, delta, NULL, array_new(0), NULL};
}

/*
La funcion delta necesita un arreglo del cual se pueda extraer un resultado,
dada una llave, en este caso la llave es {estado, simbolo}. Si la llave es
invalida, la funcion regresa un arreglo de tamaño 0 (conjunto vacio).
Note que los arreglos de tamaño 0, detienen la ejecucion de un camino.
Esta funcion depende de context.delta_mapping
*/
array_t* dynamic_delta(void* obj, const char* symbol)
{
	mapping_t key = { {obj, symbol}, NULL }, *cast = NULL;
	void** m = find_item(context.delta_mapping, &key, 0, key_cmp);

	array_t* retval = NULL;
	if(m != NULL)
	{
		cast = *m;
		retval = cast->result;
	}
	else
		retval = context.empty_set;	

	return retval;
}

/*
Mantiene registro del camino que esta atravesando el automata.
Cuando se llega a un estado final valido, se imprime un mensaje. De lo contrario
no se imprime nada, esta es la funcion que se ejecuta en cada paso del automata.
*/
void step(str_t* expr, size_t idx, void* obj)
{
	if(context.path == NULL)
	{
		context.path = array_new(str_len(expr)+1);
		set_item(context.path, 0, obj);
	}
	else
	{
		set_item(context.path, idx+1, obj);
		if(idx == str_len(expr)-1)
		{
			void** end_st = find_item
			(get_final_states(context.automaton), obj, 0, state_cmp);

			if(end_st != NULL)
			{
				printf("Cadena: \"");
				str_write(expr, stdout);
				printf("\" validada con el camino:\n");
				for_each_in(context.path, print_state);
				printf("\n\n");
			}
		}
	}
}

// Como comparar la parte "llave" de la 3-tupla, que representa un mapeo/regla
size_t key_cmp(void* m1, void* m2)
{
	size_t rv = 1;
	// printf("(%s, %s) vs (%s, %s)\n", mr1->st, mr1->sy, mr2->st, mr2->sy);
	if(m1 != NULL && m2 != NULL)
	{
		mapping_t *mr1 = m1, *mr2 = m2;
		if(!my_strcmp(mr1->st, mr2->st) && *mr1->sy == *mr2->sy)
			rv = 0;
	}

	return rv;
}

// Compara dos estados, note que los estados en este caso son cadenas
size_t state_cmp(void* s1, void* s2)
{
	size_t rv = 1;
	if(s1 != NULL && s2 != NULL)
	{
		const char *st1 = s1, *st2 = s2;
		// printf("'%s' vs '%s'\n", st1, st2);
		if(!my_strcmp(st1, st2))
			rv = 0;
	}

	return rv;
}

// Imprime un estado (una cadena)
void print_state(void* s)
{
	const char* state_id = s;
	printf(">>(%s)", state_id);
}

// Sale del programa con un mensaje personalizado
void exit_with_msg(const char* msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(EXIT_FAILURE);
}

void instruct(void)
{
	const char* instructions[] =
	{
		"Introduzca cadenas. Para salir introduzca la palabra \"exit\"",
		"Para re-cargar el automata desde el mismo archivo, escriba \"load\"",
		"Si la cadena es valida, se imprimiran los caminos generados.",
		"En caso contrario, se descarta y no se imprime nada\n",
		NULL,
	};

	const char** i = instructions;
	while(*i != NULL)
	{
		printf("%s\n", *i);
		++i;
	}

}

/*
void unit_test(void)
{
	char s[] = ">>>k>a>>b>>c>>>>d>e", sep[] = ">";
	array_t* arr = str_sep(s, sep);

	printf("list %zu\n", get_size(arr)-1);
	for(size_t i=0; i<get_size(arr)-1; ++i)
	{
		char* str = get_item(arr, i);
		printf("%s\n", str);
	}
	exit(1);
}
*/

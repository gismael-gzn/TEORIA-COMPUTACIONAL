#include "mystring.h"

// Identifica a que elemento de options[] compara igual opt_str
int opt_menu(const char *options[], const char* opt_str);

// Lee una cadena hasta algun caracter de control, imprime la cadena prompt
const char* in_read(const char* prompt);

// Imprime un arreglo de cadenas terminado en NULL
void print_charp_arr(const char *arr[]);

// Imprime una cadena con formato
void printstr(str_t* s);

/*
Son basicamente interfaces para asistir en operaciones de cadena
Hay varias reglas generales en estas funciones:

* Si una funcion crea un nuevo objeto, esta se encargara de liberar la memoria
	guardando primero una referencia a *obj, asi mismo, al crear nuevos objetos,
	estos son asignados a *obj. No deberia haber memoria perdida

* Las funciones que reciban entrada numerica haran uso de las funciones de stdlib
	strtoul o strtol, en caso de que la entrada no sea valida, digitos no base 
	10, no se notificara, en general estas funciones regresaran 0 en error, 
	asi que se tomara ese valor, se espera una cadena que represente un valor en 
	base 10 valido
*/
void newstring(str_t** obj);
void catstring(str_t** obj);
void prestring(str_t** obj);
void sufstring(str_t** obj);
void substring(str_t** obj);
void seqstring(str_t** obj);
void invstring(str_t** obj);
void powstring(str_t** obj);

// Funcion de ayuda para validar las listas de indices en seqstring
size_t check_insert(size_t* arr, size_t siz, size_t elem, size_t max);



/*
Inserta char nulo cuando se encuentra un caracter no imprimible segun ascii 
https://theasciicode.com.ar/extended-ascii-code/majuscule-c-cedilla-uppercase-ascii-code-128.html
el programa es pequeño asi que usaremos buf[] para toda entrada del programa
La razon de ser de esta funcion asumir que cualquier caracter por debajo de ' '
es un caracter de control o fin de linea en cualquier computadora
*/
const char* in_read(const char* prompt)
{
	printf("%s", prompt);
	enum {bufsz = 4096};
	static char buf[bufsz] = {0};
	char *ctrl = fgets(buf, bufsz, stdin);

	if(ctrl != NULL)
	{
		while(*ctrl >= ' ')
			++ctrl;
		*ctrl = '\0';
	}
	else
		fprintf(stderr, "%s\n", "Error al leer desde stdin"),
		exit(EXIT_FAILURE);

	return buf;
}


// Regresa el indice al que opt_str es igual dentro de options, si no regresa -1
int opt_menu(const char *options[], const char* opt_str)
{
	int i=-1, c=0;

	while(*options != NULL)
	{
		if(my_strcmp(opt_str, *options) == 0)
		{
			i = c;
			break;
		}
		++options,
		++c;
	}

	return i;
}

// Imprime un arreglo de apuntadores a char hasta encontrar el sentinela NULL
void print_charp_arr(const char *arr[])
{
	while(*arr != NULL)
		printf("%s\n", *arr),
		++arr;
}

// Imprimir una cadena, avisa si esta vacia o si es muy larga no la imprime
void printstr(str_t* s)
{
	if(str_len(s) == 0)
		printf("\n%s", 
			">>> Usando la cadena vacia, cree una nueva cadena con \"new\"\n");
	else if(str_len(s) > 4000)
		printf("\nLa cadena es muy larga [%zu], no se imprimira\n", str_len(s));
	else
		printf("\n>>> Trabajando sobre la cadena \""),
		str_write(s, stdout),
		printf("\" [%zu]\n", str_len(s));
}

// Asistente para crear una cadena, libera el objeto al que *obj apuntadaba 
void newstring(str_t** obj)
{
	const char* newval = in_read("Nueva cadena: ");

	str_t *p_free = *obj;
	*obj = str_new(newval);
	str_del(p_free);
}

// Asistente para concaternar una cadena al objeto *obj
void catstring(str_t** obj)
{
	const char* cat = in_read("Concatenar: ");
	str_ccat_raw(*obj, cat);
}

// Asistente para generar un prefijo de *obj, si se crea un nuevo objeto,
// Entonces se libera a lo que *obj apuntaba.
void prestring(str_t** obj)
{
	const char* num = 
	in_read("Remover n caracteres del final de la cadena\nn: ");

	str_t *p_free = *obj;
	char *dummy;
	size_t to = strtoul(num, &dummy, 10);

	if(to > str_len(*obj))
		printf("%zu es mayor que la longitud de la cadena [%zu], no procede\n", 
			to, str_len(*obj));
	else
		*obj = str_slice(*obj, 0, str_len(*obj)-to),
		str_del(p_free);
}

// Asistente para generar un sufijo de *obj, si se crea un nuevo objeto,
// Entonces se libera a lo que *obj apuntaba.
void sufstring(str_t** obj)
{
	const char* num = 
	in_read("Remover n caracteres del inicio de la cadena\nn: ");

	str_t *p_free = *obj;
	char *dummy;
	size_t from = strtoul(num, &dummy, 10);

	if(from > str_len(*obj))
		printf("%zu es mayor que la longitud de la cadena [%zu], no procede\n", 
			from, str_len(*obj));
	else
		*obj = str_slice(*obj, from, str_len(*obj)),
		str_del(p_free);
}

/*
Funcion para validar la insercion de "indices" para que el arreglo cumpla con:
1. Que no se repitan los indices 
2. Que ningun indice sea mayor que la longitud de la cadena
3. Que ningun indice sea menor que uno previamente insertado
Regresa 1 si el numero fue insertado y 0 en caso contrario
*/
size_t check_insert(size_t* arr, size_t siz, size_t elem, size_t max)
{
	size_t insert = 1, i = 0;

	for(; i<siz; ++i)
		if(arr[i] == elem || arr[i] > elem || elem >= max)
		{
			insert = 0;
			break;
		}

	if(insert == 1)
		arr[siz] = elem;

	return insert;
}

// Asistente para crear una subsecuencia de *obj, esta funcion hara uso de
// check_insert para asegurar que la lista de indices sea valida
void seqstring(str_t** obj)
{

	const char *instr[] =
	{
		"Se ignoraran numeros repetidos, o que generen sequencias no ordenadas",
		"Entrando en loop. Para salir: nueva linea(return), o un numero >=",
	};

	printf("%s\n", instr[0]), 
	printf("%s [%zu]\n", instr[1], str_len(*obj));

	size_t *arr = malloc(128*sizeof *arr), val, c=0;
	while(1)
	{
		const char* num = in_read("Indice ... ");
		char *dummy;

		val = strtoul(num, &dummy, 10);

		if(*num == '\0' || val >= str_len(*obj))
			break;

		c += check_insert(arr, c, val, str_len(*obj));
	}


	if(c > 0)
	{
		str_t *p_free = *obj;
		arr[c] = str_len(*obj),
		*obj = str_seq1(*obj, arr),
		str_del(p_free);
	}
	else
		printf("No se pudo generar una sequencia valida, continuado...\n");

	free(arr);
}

// Solo avisa que se esta invirtiendo la cadena de *obj
void invstring(str_t** obj)
{
	printf("Invirtiendo cadena\n");
	str_rev(*obj);
}

// Asistente para potenciar una cadena, siempre creara un nuevo objeto y
// liberara a lo que *obj apuntaba
void powstring(str_t** obj)
{
	const char *num = in_read("Potencia: ");
	char *dummy;
	long pw = strtol(num, &dummy, 10);

	str_t *p_free = *obj;
	*obj = str_pow(*obj, pw);
	str_del(p_free);
}

// Las opciones introducibles desde el menu principal en main
const char* options[] = 
{
	"new", "cc", "pf", "sf", "ss", "sq", "inv", "pw", "ex", "",
},

// Instrucciones
*menu[] = 
{
	"Programa para procesar una cadena con varias operaciones", 
	"Funciona en cadenas con caracteres de 1 byte\n",
	"introduzca el nombre de una operacion",
	"new: nueva cadena", "cc: concatenar", "pf: prefijo", "sf: sufijo",
	"ss: subcadena", "sq: subsequencia", "inv: invertir", "pw: potencia",
	"ex: salir del programa", "(return): volver a imprimir este texto", NULL,
};

int main(int argc, char const *argv[])
{
	string_module_init();

	print_charp_arr(menu);

	const char* str_opt;
	enum optc {news, cc, pf, sf, ss, sq, inv, pw, ex, tt} o;
	str_t* s = str_new(empty_string);

	do
	{
		printstr(s),
		str_opt = in_read(">>> "),
		o = opt_menu(options, str_opt);

		switch(o)
		{
			case news:
			newstring(&s);
			break;

			case cc:
			catstring(&s);
			break;

			case pf:
			prestring(&s);
			break;

			case sf:
			sufstring(&s);
			break;

			case ss:
			printf("Para generar subsequencia\n"),
			sufstring(&s),
			prestring(&s);
			break;

			case sq:
			seqstring(&s);
			break;

			case inv:
			invstring(&s);
			break;

			case pw:
			powstring(&s);
			break;

			case ex:
			printf("\tsaliendo...\n");
			str_del(s);
			break;

			case tt:
			print_charp_arr(menu);
			break;

			default:
			printf("\tla accion \"%s\" no es valida\n", str_opt);
			break;

		}

	}while(o != ex);

	return 0;
}

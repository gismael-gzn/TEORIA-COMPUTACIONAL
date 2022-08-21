#include "mystring.h"
#include "automata.h"

/*
En un principio algunas partes del programa se ven raras, como las funciones
alph, smap1, smap2, lo hice de esa manera para probar algo distinto con las
capacidades que tiene el lenguaje C, los estados del automata son 
*/

/*
Para ejemplificar la versatilidad? del automata
Ejemplo algo rebuscado, pero se muestra como se puede utilizar el automata
Este tipo representa un estado del automata, en teoria esta hecho para mostrar
las aplicaciones que puede tener un automata fd, ya que por ejemplo, se podria
inicializar una coleccion de objetos de este tipo, y cada uno con una funcion
que realice una operacion distinta sobre los atributos del objeto.
*/
typedef struct
{
	int id;
	void (*function)(int id);
}some_state;

// Que es lo que hace cada estado del automata
void state_function(int id)
{
	printf(">>(estado %d)", id);
}

// Mapa de objetos estado para el automata1
some_state* smap1(int elem)
{
	static some_state map[] = 
	{
		{0, state_function}, {1, state_function}, 
		{2, state_function}, {3, state_function},
	};
	return map+elem;
}

// Mapa de objetos estado para el automata2
some_state* smap2(int elem)
{
	static some_state map[] = 
	{
		{0, state_function}, {1, state_function}, {2, state_function}, 
		{3, state_function}, {4, state_function},
	};
	return map+elem;
}

// Con argumento 0 regresa el alfabeto {0,1}, con 1, libera el alfabeto.
strset_t* alph(int use_case)
{
	static const char* alphabet1[] = {"0", "1", NULL};
	static strset_t* alph = NULL;

	if(alph == NULL && use_case == 0)
		alph = set_from_arr(alphabet1);

	else if(alph != NULL && use_case == 1)
		set_strong_del(alph), alph = NULL;

	return alph;
}

// Funcion Delta para el automata del ejercicio 2
// Mapea desde (estado, simbolo) -> estado
void* Delta1(void* arg, const char* chr)
{
	some_state* curr_state = arg;
	curr_state->function(curr_state->id);

	enum cases {oh='0', on='1'};
	if(arg == smap1(0))
		switch(*chr)
		{
			case oh:
			return smap1(2);

			case on:
			return smap1(1);
		}
	else if(arg == smap1(1))
		switch(*chr)
		{
			case oh:
			return smap1(3);

			case on:
			return smap1(0);
		}
	else if(arg == smap1(2))
		switch(*chr)
		{
			case oh:
			return smap1(0);

			case on:
			return smap1(3);
		}
	else if(arg == smap1(3))
		switch(*chr)
		{
			case oh:
			return smap1(1);

			case on:
			return smap1(2);
		}
	else
		fprintf(stderr, "%s\n", "Argumento no valido");

	return NULL;
}

// Funcion Delta para el automata del ejercicio 2
// Mapea desde (estado, simbolo) -> estado
void* Delta2(void* arg, const char* chr)
{
	some_state* curr_state = arg;
	curr_state->function(curr_state->id);

	enum cases {oh='0', on='1'};
	if(arg == smap2(0))
		switch(*chr)
		{
			case oh:
			return smap2(1);

			case on:
			return smap2(2);
		}
	else if(arg == smap2(1))
		switch(*chr)
		{
			case oh:
			return smap2(1);

			case on:
			return smap2(3);
		}
	else if(arg == smap2(2))
		switch(*chr)
		{
			case oh:
			return smap2(1);

			case on:
			return smap2(2);
		}
	else if(arg == smap2(3))
		switch(*chr)
		{
			case oh:
			return smap2(1);

			case on:
			return smap2(4);
		}
	else if(arg == smap2(4))
		switch(*chr)
		{
			case oh:
			return smap2(1);

			case on:
			return smap2(2);
		}
	else
		fprintf(stderr, "%s\n", "Argumento no valido");

	return NULL;
}

// Estado general del programa
enum {bufsz = 4096};
struct
{
	int opt;
	char buf[bufsz], *cmds[4];
}program = { 0, {0}, {"1", "2", "q", NULL}, };

// Checa si una cadena esta formada sobre un alfabeto, se asumen simbolos de 1byte 
int validate_string(strset_t* alph, str_t* str)
{
	size_t i, n;
	for(i=0, n=str_len(str); i<n; ++i)
	{
		const char* mem = str_arr(str)+i;
		if( !Element(alph, mem, 1) )
		{
			i = 0;
			break;
		}
	}
	return i;
}

// Lee entrada desde stdin e imprime un "prompt" personalizable
void usr_input(const char* p)
{
	printf("%s", p);
	if(fgets(program.buf, bufsz, stdin) != NULL)
	{
		char* terminate = program.buf;
		while(*terminate != '\n')
			++terminate;
		*terminate = '\0';
	}
}

// Lee entrada desde stdin y busca a que opcion del programa corresponde
void fmt_read(void)
{
	usr_input(">> ");
	program.opt = 0;

	char** words = program.cmds;
	while(*words != NULL)
	{
		if(my_strcmp(*words, program.buf) == 0)
			break;
		++program.opt, ++words;
	}

}

// main
int main(int argc, char const *argv[])
{
	string_module_init();

	automata_t* ats[] =
	{
		Automata(smap1(0), alph(0), Delta1, smap1(0), smap1(0)),
		Automata(smap2(0), alph(0), Delta2, smap2(0), smap2(4)),
	};
	
	printf("1: automata1, 2: automata2, q: quitar\n");
	while(program.opt != 2)
	{
		str_t* feed = NULL;
		fmt_read();

		switch(program.opt)
		{
			case 0:
			case 1:
			set_writef(alph(0), stdout, "Introduce cadena valida sobre: ");
			usr_input("... ");
			feed = str_new(program.buf);

			if(validate_string(alph(0), feed))
			{
				if(determine_acceptance(ats[program.opt], feed))
					printf("\nSe llega a estado de aceptacion\n");
				else
					printf("\nNo se llega a estado de aceptacion\n");
			}
			else
			{
				printf("\"%s\" no es valida sobre el alfabeto ", str_arr(feed));
				set_writef(alph(0), stdout, "");
			}

			str_del(feed);

			break;

			case 2:
			printf("saliendo...\n");
			break;

			default:
			printf("\"%s\" no es valido\n", program.buf);
			break;
		}
	}

	alph(1);
	del_automata(ats[0]), del_automata(ats[1]);

	return 0;
}

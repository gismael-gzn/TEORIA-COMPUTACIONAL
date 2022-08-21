#include "mystring.h"

const char* instrucciones[] = {
	"Escriba un comando para operacion sobre lenguajes: ",
	"Se guardaran 3 conjuntos A(por el usuario), B(por el usuario) y",
	"C = A <op> B (para operaciones binarias)",
	"C = <op> C (para operaciones unarias)",
	"El programa liberara memoria tras 24 operaciones seguidas",
	"\n[a-b]: sobre-escribir en los conjuntos A o B (vacios al iniciar)",
	"un:\tunion", "in:\tinterseccion", "df:\tdiferencia", "cc:\tconcatenacion",
	"pw:\tpotencia", "klp:\tKleene mas", "kls:\tKleene estrella", 
	"pr:\timprimir conjuntos A, B, C",  "ex:\tsalir del programa",
	"(tecla: return/enter):\tvolver a imprimir este texto", NULL,
},

*opts[] = 
{"un", "in", "df", "cc", "pw", "klp", "kls", "pr", "ex", "", "a", "b", NULL,};

static void instruct(void)
{
	printf("\n");
	const char** p = instrucciones;
	while(*p != NULL)
	{
		printf("%s\n", *p);
		++p;
	}
	printf("\n");
}

static void print_set(strset_t* set, const char* set_name)
{
	char buf[512] = {0};
	if(Count(set) > 1000)
		printf("Conjunto muy grande para imprimir |%s| = %zu\n", 
			set_name, Count(set));
	else
		sprintf(buf, "|%s| = %zu, %s = ", set_name, Count(set), set_name),
		set_writef(set, stdout, buf);
	// printf("\n");
}

static int opt(const char* prompt)
{

	printf("%s", prompt);

	enum {bufsz=8192};
	static char buf[bufsz] = {0};
	if (fgets(buf, bufsz, stdin) == NULL)
		fprintf(stderr, "Error al leer stdin\n"),
		abort();
	else
		(my_strlen(buf)-1)[buf] = '\0';

	const char** p = opts;
	int pos = 0;
	while(*p != NULL)
	{
		if(my_strcmp(buf, *p) == 0)
			break;
		++pos, ++p;
	}

	return pos;
}

static strset_t* read_set(void)
{

	printf("Leyendo cadenas, presione la tecla enter/return para salir\n");

	strset_t* set = set_new();

	enum {bufsz=8192};
	static char buf[bufsz] = {0};
	do
	{
		size_t l;
		printf(": ");
		if (fgets(buf, bufsz, stdin) == NULL)
			fprintf(stderr, "Error al leer stdin\n"),
			abort();
		else
		{
			(l = my_strlen(buf)-1)[buf] = '\0';
			if(l > 0)
				set_add_raw(set, buf);
		}

	}while(*buf != '\0');

	return set;
}

#define max_num 13
#define max_num_for_big_sets 2
#define big_set_size 150
#define too_big_for_pow 1600
#define big_kleene_on_big_set 9
#define big_set_for_kleene 5
#define big_kleene 12
static long num(size_t n)
{
	printf("num > ");

	enum {bufsz=8192};
	static char buf[bufsz] = {0};
	if (fgets(buf, bufsz, stdin) == NULL)
		fprintf(stderr, "Error al leer stdin\n"),
		abort();
	else
		(my_strlen(buf)-1)[buf] = '\0';

	char* e = NULL;

	long num = strtol(buf, &e, 10);

	if(num > max_num)
		fprintf(stderr, "%ld es muy grande, tomando valor 1\n", num),
		num = 1;
	if(n > too_big_for_pow || (num > max_num_for_big_sets && n > big_set_size)
		|| (n > big_set_for_kleene && num > big_kleene_on_big_set)
		|| n > big_kleene)
		fprintf(stderr, "El conjunto es muy grande para potenciar\n"),
		num = 1;

	return num;
}

#include <assert.h>

#define MEM_MAX 24
int main(int argc, char const *argv[])
{
	string_module_init();

	strset_t *(*fbin[])(strset_t*, strset_t*) = {
		Union, Intersection, Difference, Cartesian,
	}, 
	*(*funa[])(strset_t*, long) = {
		Power, Kleene_plus, Kleene_star,
	};

	int p = 0, init_flag = 1;
	enum option { un, in, df, cc, pw, klp, kls, pr, ex, man, a, b}o=man;
	struct {strset_t* set; enum option o;}memory[MEM_MAX] = {0};

	strset_t *A,*B,*C;
	do 
	{
		if(init_flag)
		{
			memory[p].set = A = set_new(); memory[p].o = a; ++p;
			memory[p].set = B = set_new(); memory[p].o = a; ++p;
			memory[p].set = C = set_new(); memory[p].o = a;	++p;
			init_flag = 0;
		}	
		switch(o) 
		{
			case un: case in: case df: case cc:
			memory[p].set = C = fbin[o](A, B); memory[p].o = o; ++p;
			break;

			case pw: case klp: case kls: 
			memory[p].set = C = funa[o-4](C, num(Count(C))); memory[p].o = o; ++p;
			break;

			case pr:
			print_set(A, "A"), print_set(B, "B"), print_set(C, "C");
			break;

			case man: instruct(); break;

			case a: memory[p].set = A = read_set(); memory[p].o = o; ++p; break; 
			case b: memory[p].set = B = read_set(); memory[p].o = o; ++p; break;

			default: printf("Accion indefinida\n"); break;
		}
		if(p == MEM_MAX)
		{
			fprintf(stderr, "Limite de operaciones alcanzado\nResultados:\n");
			print_set(A, "A"), print_set(B, "B"), print_set(C, "C");
			
			if(p >= 1)
				while(p-- != 0)
					if( memory[p].o <= df )
						set_weak_del(memory[p].set);
					else
						set_strong_del(memory[p].set);
			p = 0;
			init_flag = 1;
		}
	}while((o = opt(">>> ")) != ex);

	return 0;
}




































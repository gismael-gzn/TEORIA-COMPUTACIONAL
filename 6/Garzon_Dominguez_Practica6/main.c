#include "automata.h"
#include "memutils.h"

void instruct(void);

typedef int (*production)(str_t*, size_t);
int in_grammar(str_t* str, strset_t* terminals, production initial);

int 
S(str_t* str, size_t curr),
A(str_t* str, size_t curr),
B(str_t* str, size_t curr);

int main(int argc, char const *argv[])
{
	mem_utils(free_at_exit);

	instruct();
	strset_t* memo = set_new();
	str_t *exit_string = str_new("exit"), *input = NULL;

	while(input = str_input(512, ">> "), 1)
		if(str_hash(input) != str_hash(exit_string)) 
			printf("%s\n",
				in_grammar(input, memo, S)? 
				"Aceptado por la gramatica!":
				"No aceptado por la gramatica!"
			);
		else 
			break;

	set_writef(memo, stdout, "Cadenas en L(G) = ");

	return 0;
}

int S(str_t* str, size_t curr)
{
	if(str_len(str) == curr) return 1;
	switch(str_idx(str, curr))
	{
		case 'a': return B(str, curr+1);
		case 'b': return A(str, curr+1);
		default: break;
	}
	return 0;
}

int A(str_t* str, size_t curr)
{
	if(str_idx(str, curr) == 'a') return S(str, curr+1);
	else return 0;
}

int B(str_t* str, size_t curr)
{
	if(str_idx(str, curr) == 'b') return S(str, curr+1);
	else return 0;
}

int in_grammar(str_t* str, strset_t* memo, production initial)
{
	int retval = 0;
	if(Element_ref(memo, str))
		retval = 1;
	else
		(retval = initial(str, 0))? set_add_ref(memo, str): 0;
	return retval;
}

void instruct(void)
{
	printf("Introduzca cadenas.\nPara salir escriba 'exit'\n");
}

#ifndef STACK_H__
#define STACK_H__

#include "node_definitions.h"
#include "container.h"

// Embed stack type into your struct
#define STACK_TYPE(type)\
CONTAINER_HEAD(eml_stack_type___);\
type dummy;\

// Directly typedef a stack containing SINGLY_ND type
#define def_stack(name, type)\
typedef struct name name;\
struct name\
{\
	CONTAINER_HEAD(eml_stack_type___);\
	type dummy;\
}\

// Get top of the stack aka first element
#define s_top(stack) (get_head(stack))

// Add node in top of the stack
#define s_add(stack, node)\
{\
	len_incr(stack);\
	sg_add(s_top(stack), node);\
}\

// Pop first element and save it in a simple pointer "save"
#define s_pop(stack, save)\
{\
	save = s_top(stack);\
	len_decr(stack);\
	mv_next(s_top(stack));\
}\

// Always initialize a stack after allocation
#define s_init(stack)\
{\
	memset_fun(stack, 0, sz(*stack));\
	sg_dummy((&(stack)->dummy));\
}\


#endif // STACK_H__

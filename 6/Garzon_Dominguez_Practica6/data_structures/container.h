#ifndef HAVE_CONTAINER_H__
#define HAVE_CONTAINER_H__

/*
* Macros in this header file describe operations that can be performed over
* containers, a container it's a type that contains any type of list: stacks
* queues, doublys, arrays, etc, and keeps track on the size of the list.

* If the macro MLISTS_WANT_GENERIC_FUNCTIONS is defined (check mlists_config.h),
* then the container type will also keep a pointer to a forward declarated type,
* this type never gets completely defined, and it will serve the purpose of
* writting generic macros for the containers using _Generic from C11 standard.
* This feature is not implemented yet, and there's no guarantee that it will
* ever be, regardless of that, if MLISTS_WANT_GENERIC_FUNCTIONS ins't defined,
* then the containers are still completly functional and don't have any extra
* overhead by saving a pointer to a forward declared type.
* NOTE: MLISTS_WANT_GENERIC_FUNCTIONS may also define other useful stuff to
* easily use the generic functions, (if ever implemented)
*/

#include <stdlib.h>

#include "mlists_config.h"


#if defined (MLISTS_WANT_GENERIC_FUNCTIONS)

typedef struct eml_queue_type___* eml_queue_type___;
typedef struct eml_stack_type___* eml_stack_type___;
typedef struct eml_singl_type___* eml_llist_type___;
typedef struct eml_array_type___* eml_array_type___;

#	define CONTAINER_HEAD(c_type) size_t len; c_type type;

#else

#	define CONTAINER_HEAD(c_type) size_t len;

#endif

// Generic iterator, caller must cast this if there's ever an attempt to
// access an attribute of the contained type
typedef struct mlgeneric_iter_t 
{ 
	struct mlgeneric_iter_t *right, *left; 
}mlgeneric_iter_t;

// Caller must not use these
#define len_decr(container) (--(container)->len)

#define len_incr(container) (++(container)->len)


// Get the length of a container
#define get_len(container) ((container)->len)

//For doubly linked lists 
#define get_head(list) ((list)->dummy.right)

#define get_tail(list) ((list)->dummy.left)

// Following 5 macros are conditionals to easily iterate trhough a container's list

// From a double pointer iterator, checks if the iterator has reached dummy node
#define iauto(container, iterator) ((iterator) != &(container)->dummy)

// Given a number "num", checks if num is smaller than the lenght of the container
#define nauto(container, num) ((num) < get_len(container))

// Must use when using lists as cirular in bucles
// direction of the iteration (left, right)
#define i_guard(cont, iter, direction)\
{\
	if((*iter) == &(cont)->dummy)\
		iter = &(*iter)->direction;\
}\

// Check wether a container is empty (By checking head pointer or numerically)
#define lempty(container) (get_head(container) == &(container)->dummy)
#define nempty(container) (get_len(container) == 0)


#if defined(MLISTS_WANT_GENERIC_FUNCTIONS)
enum iterator_direction {MLI_FORWARD, MLI_BACKWARDS};
#endif

/*
This allows the caller to use other than stdlib and string implementations
of following functions, although the only module that makes complete of these is
array.h, the rest of the types only use memset_fun to zero out the container
*/
extern void* (*malloc_fun)(size_t size);

extern void* (*realloc_fun)(void *b, size_t bytes);

extern void (*free_fun)(void* ptr);

extern void* (*memset_fun)(void *b, int c, size_t len);

extern void* (*memmove_fun)(void *dst, const void *src, size_t len);

#define sz sizeof


#endif // HAVE_CONTAINER_H__

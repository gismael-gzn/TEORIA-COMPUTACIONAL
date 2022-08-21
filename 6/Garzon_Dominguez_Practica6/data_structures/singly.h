#ifndef SINGLY_H__
#define SINGLY_H__

#include "node_definitions.h"
#include "container.h"

// Embed SINGLY_TYPE into your struct
#define SINGLY_TYPE(type)\
CONTAINER_HEAD(eml_llist_type___)\
type dummy;\

// Directly typedef a stack containing SINGLY_ND type
#define def_linked(name, type)\
typedef struct name name;\
struct name\
{\
	CONTAINER_HEAD(eml_llist_type___;);\
	type dummy;\
}\


/*
* save is a double pointer to the node type of list
* this macro moves *save n positions forward
* it works in a circular way and skips dummy node
*/
#define sl_get(list, save, n)\
{\
	if(save == NULL)\
		save = &get_head(list);\
	for(size_t i___=0; i___<(n); ++i___)\
	{\
		save = &(*save)->right;\
		i_guard(list, save, right);\
	}\
}\


// inserts node at *pos
#define sl_ins(list, pos, node)\
{\
	len_incr(list);\
	sg_add(*pos, node);\
}\


// pops pos node for further processing
#define sl_pop(list, pos)\
{\
	len_decr(list);\
	mv_next(*pos);\
}\

// Always initialize after allocation
#define sl_init(s_list)\
{\
	memset_fun(s_list, 0, sz(*s_list));\
	sg_dummy(&(s_list)->dummy);\
}\


#endif // SINGLY_H__

#ifndef DOUBLY_H__
#define DOUBLY_H__

#include "node_definitions.h"
#include "container.h"

// Embed DOUBLY_TYPE into your struct
#define DOUBLY_TYPE(type)\
CONTAINER_HEAD(eml_llist_type___);\
type dummy;\

// Directly typedef a doubly linked list containing a DOUBLY_ND type
#define def_dlinked(name, type)\
typedef struct name name;\
struct name\
{\
	CONTAINER_HEAD(eml_llist_type___);\
	type dummy;\
}\

/*
* save is a double pointer to the node type of list
* this macro moves *save n positions forward
* it works in a circular way and skips dummy node
*/
#define dl_fwd(list, save, n)\
{\
	if(save == NULL)\
		save = &get_head(list);\
	for(size_t i___=0; i___<(n); ++i___)\
	{\
		save = &(*save)->right;\
		i_guard(list, save, right);\
	}\
}\

/*
* save is a double pointer to the node type of list
* this macro moves *save n positions forward
* it works in a circular way and skips dummy node
*/
#define dl_bwd(list, save, n)\
{\
	if(save == NULL)\
		save = &get_tail(list);\
	for(size_t i___=0; i___<(n); ++i___)\
	{\
		save = &(*save)->left;\
		i_guard(list, save, left);\
	}\
}\

// Inserts node before *pos
#define dl_insb(list, pos, node)\
{\
	len_incr(list);\
	db_link_befor(*pos, node);\
	db_link_left_and_right(node);\
}\

// Inserts node after *pos
#define dl_insa(list, pos, node)\
{\
	len_incr(list);\
	db_link_after(*pos, node);\
	db_link_left_and_right(node);\
}\

// If &tail pointer received, inserts node at tail
// Otherwise inserts it before *pos
#define dl_ins(list, pos, node)\
{\
	len_incr(list);\
	if(pos == &get_tail(list))\
		db_link_after(*pos, node)\
	else\
		db_link_befor(*pos, node);\
	db_link_left_and_right(node);\
}\

// Pop the pointer pointed to by *pos, save that value for further processing
// pos pointer gets moved right or left 
// depending which direction was being iterated
#define dl_pop(list, pos, save)\
{\
	len_decr(list);\
	save = *pos;\
	pos = &(*pos);\
	db_del(save);\
}\

// Always initialize after allocation
#define dl_init(s_list)\
{\
	memset_fun(s_list, 0, sz(*s_list));\
	db_dummy((&(s_list)->dummy));\
}\


#endif // SINGLY_H__

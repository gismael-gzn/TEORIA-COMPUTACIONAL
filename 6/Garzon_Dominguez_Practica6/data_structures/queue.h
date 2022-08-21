#ifndef QUEUE_H__
#define QUEUE_H__

#include "node_definitions.h"
#include "container.h"

// Embed queue type into your struct
#define QUEUE_TYPE(type)\
CONTAINER_HEAD(eml_queue_type___);\
type *tail, dummy\

// Directly typedef a stack containing SINGLY_ND type
#define def_queue(name, type)\
typedef struct name name;\
struct name\
{\
	CONTAINER_HEAD(eml_queue_type___);\
	type *tail, dummy;\
}\

// Get queue's first element
#define q_first(queue) get_head(queue)

// Get queue's last elemnt
#define q_last(queue) (queue)->tail

// Add node in queue's tail
#define q_add(queue, node)\
{\
	len_incr(queue);\
	sg_add(q_last(queue)->right, node);\
	mv_next(q_last(queue));\
}\

// Pop first element and save it in a simple pointer "save"
#define q_pop(queue, save)\
{\
	save = q_first(queue);\
	len_decr(queue);\
	mv_next(q_first(queue));\
}\

// Always initialize after allocation
#define q_init(queue)\
{\
	memset_fun(queue, 0, sz(*queue));\
	(queue)->tail = &(queue)->dummy;\
	sg_dummy((queue)->tail);\
}\


#endif // QUEUE_H__

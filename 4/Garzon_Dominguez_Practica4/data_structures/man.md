# mlists.h

**To use it simply #include "mlists.h" where needed**

This library is a collection of helper macros to implement generic type

linear lists, it includes the following data structures in the form of

containers:

- Stacks
- Queues
- Singly linked lists
- Doubly linked lists
- Arrays

## Node types

There are 2 types of nodes, singly and doubly, which are structs with either 

one or two pointers to their own type, plus the members specified by the user

### How to define a node type

**Singly nodes**
```C
SINGLY_ND(type)
def_singly(type_name, members...)
```
**Doubly nodes**
```C
DOUBLY_ND(type)
def_doubly(type_name, members...)
```

## Containers

A container is a construct which keeps track of the list and its lenght.

There are some general operations that can be performed on containers.

And we can define new operations which mutate the contained type/list.

This allows to implement different data types in the form of containers

### How to define a container type

**Stacks**
```C
*node_type must be singly type*
STACK_TYPE(node_type)
def_stack(type_name, node_type)
```

**Queues**
*node_type must be singly type*
```C
QUEUE_TYPE(node_type)
def_queue(type_name, node_type)
```

**Singly linked lists**
*node_type must be singly type*
```C
SLINKED_TYPE(node_type)
def_slinked(type_name, node_type)
```

**Doubly linked lists**
*node_type must be doubly type*
```C
DLINKED_TYPE(node_type)
def_slinked(type_name, node_type)
```

**Arrays**
*contained_type is any type*
```C
ARRAY_TYPE(contained_type)
def_array(type_name, contained_type)
```

#### Examples on how to define your own container type
**def_type(type_name, ...) macros forward declare type_name**
**TYPE_ND(type) and TYPE_TYPE(node_type) are suitable to embed into structs**

**For linked types**
```C
//Singly node my_type defined:
typedef struct my_type {
	SINGLY_ND(struct my_type);
	char *fname, *sname;
	uint64_t hash, id;
}my_type;

//Stack container my_container defined:
typedef struct my_container {
	STACK_TYPE(my_type);
	//You could add more members here
}my_container;
```

*Which is the same as:*
```C
def_singly(my_type, char *fname, *sname; uint64_t hash, id;);
def_stack(my_container, my_type);
```

**For arrays**
```C
//Array containing a pointer to char (char*), with typename my_array_t defined:
typedef struct my_array_t {
	ARRAY_TYPE(char);
}my_array_t;
```

*This is the same as:*
```C
def_array(my_array_t, char);
```

## Macros and how to
All macros receive a pointer to the container as their first argument
To get predictable results, always use the macros that are intended to be used
for a given type of container

Generic macros

## Memory management and manipulation

Some macros make use of memory functions, be it for allocation, freeing,

setting, copying, and moving. User is able to set these functions to whatever

they need by using some pointers to functions provided:

These pointers to function are qualified as extern and have the same prototype 

as the standard C library functions that they are set to by default, being:

The following table describes the name of the pointer, standard function it

points to, and wether a container makes use of it or doesn't.

| Pointer     | Std C   | Stacks | Queues | Singly | Doubly | Arrays |
|-------------|---------|--------|--------|--------|--------|--------|
| malloc_fun  | malloc  | NO     | NO     | NO     | NO     | YES    |
| realloc_fun | realloc | NO     | NO     | NO     | NO     | YES    |
| free_fun    | free    | NO     | NO     | NO     | NO     | YES    |
| memset_fun  | memset  | YES    | YES    | YES    | YES    | YES    |
| memmove_fun | memmove | NO     | NO     | NO     | NO     | YES    |



















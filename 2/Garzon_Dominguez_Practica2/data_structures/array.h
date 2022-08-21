#ifndef ARRAY_H__
#define ARRAY_H__

#include "container.h"

// Embed an ARRAY_TYPE into your struct, "type" is the type of the array
// i.e int, double, uint64_t, struct etc
#define ARRAY_TYPE(type)\
CONTAINER_HEAD(eml_array_type___)\
size_t slots;\
type* array;\

// Typedef name and define struct name which contains an array of type "type"
#define def_array(name, type)\
typedef struct name name;\
struct name\
{\
	CONTAINER_HEAD(eml_array_type___);\
	size_t slots;\
	type* array;\
}\

// Used in this .h file, caller must not use these
#define a_isz ARRAY_INITIAL_SIZE
#define a_slots(cont) ((cont)->slots)
#define slo_decr(cont) (--a_slots(cont))
#define slo_incr(cont) (++a_slots(cont))
#define arr(cont) (cont)->array
#define ini_allocsz(cont) (a_isz * sz*arr(cont))
#define siz_allocsz(cont, siz) ((siz) * sz*arr(cont))
#define dup_allocsz(cont) (2*(a_slots(cont)==0? a_isz: a_slots(cont))*sz*arr(cont))
#define shr_allocsz(cont) (get_len(cont)*sz*arr(cont))

//Avoid floating point exceptions, used in modulo operations
#define avoid_fpe(l) (l<1? 1: l)

// Get element n or index element n from array, 
// the macro ensures that n is within the range of the array's lenght
#define a_get(cont, n) (arr(cont)+( (n) % avoid_fpe(get_len(cont))))
#define a_idx(cont, n) (arr(cont)[ (n) % avoid_fpe(get_len(cont)) ])


/*
Given a pointer of type arr(cont), it evaluates to the index ptr points to
respect to arr(cont), if the pointer it's outside the range (array, lenght-1),
then indexing or any other operation is bound to undefined behaviour
*/
#define idx(cont, ptr) ((size_t)(ptr - arr(cont)))

// Always initialize after allocation
#define a_init(cont)														\
{																			\
	memset_fun(cont, 0, sz*cont),											\
	a_slots(cont) = a_isz,													\
	arr(cont) = malloc_fun(ini_allocsz(cont));								\
}

#define a_init_siz(cont, siz)												\
{																			\
	memset_fun(cont, 0, sz*cont),											\
	a_slots(cont) = siz,													\
	arr(cont) = malloc_fun(siz_allocsz(cont, siz));							\
}


// Reset an array to an empty state
#define a_del(cont)															\
{																			\
	free_fun(arr(cont)),													\
	memset_fun(cont, 0, sz*cont);											\
}																			\


// Insert elem, which is always going to be inserted at i%(length+1)
#define a_ins(cont, i, elem)												\
{																			\
	size_t l=get_len(cont), m=(i)%(l+1);									\
	if(a_slots(cont) == 0)													\
		a_slots(cont) = get_len(cont),										\
		arr(cont) = realloc_fun(arr(cont), dup_allocsz(cont));				\
	memmove_fun(arr(cont)+m+1, arr(cont)+m, sz*arr(cont)*(l-m));			\
	arr(cont)[m] = elem, slo_decr(cont), len_incr(cont);					\
}																			\


// Insert at the end of the array
#define a_add(cont, elem)													\
{																			\
	if(a_slots(cont) == 0)													\
		a_slots(cont) = get_len(cont),										\
		arr(cont) = realloc_fun(arr(cont), dup_allocsz(cont));				\
	arr(cont)[get_len(cont)] = elem, slo_decr(cont), len_incr(cont);		\
}\

/*
Pops the element given an index, index gets modulated to be within the range
of the array's lenght
*/
#define a_pop(cont, i)														\
{																			\
	size_t l=get_len(cont), m=(i)%avoid_fpe(l);								\
	memmove_fun(arr(cont)+m, arr(cont)+m+1, sz*arr(cont)*(l-m)),			\
	slo_incr(cont), len_decr(cont);											\
	if(get_len(cont) == a_slots(cont))										\
		a_slots(cont) = 0,													\
		arr(cont) = realloc_fun(arr(cont), shr_allocsz(cont));				\
}\

// Grow the available slots in the array by new_elems
#define a_grow(cont, new_elems)												\
{																			\
	a_slots(cont) += (new_elems),											\
	arr(cont) = realloc_fun(arr(cont),										\
		sz*arr(cont)*(get_len(cont)+a_slots(cont)));						\
}																			\

// Shrink the array to size new_total of elements
// if new_total is 0 the array gets deleted, the new_total must be smaller than 
// lenght + slots (not inclusive), otherwise, no action is performed
#define a_shrn(cont, new_total)												\
{																			\
	if(new_total == 0)														\
		a_del(cont)															\
	else if(new_total < get_len(cont)+a_slots(cont))						\
		a_slots(cont) = 0,													\
		get_len(cont) = new_total,											\
		realloc_fun(arr(cont), sz*arr(cont)*(new_total));					\
}																			\

// Concatenate the contents of cont2 to cont1, it works even if cont1==cont2
#define a_ccat(cont1, cont2)												\
{																			\
	size_t l1=get_len(cont1), l2=get_len(cont2), s=a_slots(cont1);			\
	if(s < l2)																\
		a_slots(cont1) = 0,													\
		arr(cont1) = realloc_fun(arr(cont1), sz*arr(cont1)*(l1+l2));		\
	else																	\
		a_slots(cont1) -= l2;												\
	memmove_fun(arr(cont1)+l1, arr(cont2), sz*arr(cont2)*l2);				\
	get_len(cont1) += l2;													\
}																			\


// Concatenate n elements of contained size from mem to cont1
#define a_ccat_raw(cont1, mem, elements)									\
{																			\
	size_t l1=get_len(cont1), l2=elements, s=a_slots(cont1);				\
	if(s < l2)																\
		a_slots(cont1) = 0,													\
		arr(cont1) = realloc_fun(arr(cont1), sz*arr(cont1)*(l1+l2));		\
	else																	\
		a_slots(cont1) -= l2;												\
	memmove_fun(arr(cont1)+l1, mem, sz*arr(cont1)*l2);						\
	get_len(cont1) += l2;													\
}																			\

// Concatenate n elements of contained size from mem to cont1
// Using special memmove function 
#define a_ccat_raw_with(cont1, mem, elements, using_memmove)				\
{																			\
	size_t l1=get_len(cont1), l2=elements, s=a_slots(cont1);				\
	if(s < l2)																\
		a_slots(cont1) = 0,													\
		arr(cont1) = realloc_fun(arr(cont1), sz*arr(cont1)*(l1+l2));		\
	else																	\
		a_slots(cont1) -= l2;												\
	using_memmove(arr(cont1)+l1, mem, sz*arr(cont1)*l2);					\
	get_len(cont1) += l2;													\
}																			\

/*
Chop an interval of the array and move items located in arr+to and forward
to arr+from, from-to slots left available, the macro ensures that:
from<to and both are within the range of the container's lenght, caller can
chop the entire list which is equivalent to a_del, if the space gets shrinked 
by half, the array gets reallocated to half the original size
*/
#define a_chop(cont, from, to)												\
{																			\
	size_t l=avoid_fpe(get_len(cont)), from_=(from)%l, to_=(to)%(l+1),		\
	interval;																\
	if(from_ > to_)															\
	 	to_ ^= from_, from_ ^= to_, to_ ^= from_;							\
	interval = to_-from_;													\
	if(interval != 0)														\
	 	memmove_fun(arr(cont)+from_, arr(cont)+to_, sz*arr(cont)*(l-to_)),	\
	 	a_slots(cont) += interval,											\
	 	get_len(cont) -= interval;											\
	if(interval == l)														\
		a_del(cont)															\
	else if(get_len(cont) == a_slots(cont))									\
		a_slots(cont) = 0,													\
		arr(cont) = realloc_fun(arr(cont), shr_allocsz(cont));				\
}

//Conditional to be used in loops, given a pointer of type arr(cont)
#define a_auto(cont, i) ((i)<arr(cont)+get_len(cont))

//For automatic iteration using a pointer type* i
/*
Notice that any realloc that changes the array pointer may left i pointing to
a non valid adress, it's adviced to use numeric control variables in cases of
insertions and deletions within the array
*/
#define array(cont, type, i)\
type i=a_get(cont, 0); a_auto(cont, i); ++i


// These depend on config file
#if defined(HAVE_GNU_COMPILER) && defined(MLISTS_WANT_TYPEOF_ITERATORS)

#	undef array
#	define array(cont, i)\
	(typeof(arr(cont)))* i=a_get(cont, 0); a_auto(cont, i); ++i

#elif defined(HAVE_GNU_COMPILER) && defined(MLISTS_WANT_TYPEOF_ITERATORS) 
	&& defined(MLISTS_WANT_FANCY_FOR)

#	undef array
#	define array(cont, i)\
	((typeof(arr(cont)))* i=a_get(cont, 0); a_auto(cont, i); ++i)

#endif //defined(HAVE_GNU_COMPILER)


#endif // ARRAY_H__

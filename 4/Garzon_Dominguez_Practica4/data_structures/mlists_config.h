#ifndef MLISTS_CONFIG_H__
#define MLISTS_CONFIG_H__

// This file affects code on most files of this project
// Set to 0 if you don't want a feature, set to 1 if you do want it

// initial slots of array when initialized, use values >= 1
// That is because malloc 0 and realloc 0 are usually implementetion defined
#define ARRAY_INITIAL_SIZE 2ULL

// Generic functions, not implemented yet
#define ML_WANT_GENERIC_FUN 0

// Typeof iterator for arrays, 
// __GNUC__ must be defined
#define ML_WANT_TYPEOF_ITER 0

// Fancy for loops for arrays
// ML_WANT_TYPEOF_ITER must be set to 1
// __GNUC__ must be defined
#define ML_WANT_FANCY_FOR	0


#if defined(__GNUC__)
#	define HAVE_GNU_COMPILER
#endif

#if (ML_WANT_GENERIC_FUN == 1)
#	define MLISTS_WANT_GENERIC_FUNCTIONS
#endif

#if (ML_WANT_TYPEOF_ITER == 1)
#	define MLISTS_WANT_TYPEOF_ITERATORS
#endif

#if (ML_WANT_FANCY_FOR == 1)
#define MLISTS_WANT_FANCY_FOR
#endif


#endif // MLISTS_CONFIG_H__









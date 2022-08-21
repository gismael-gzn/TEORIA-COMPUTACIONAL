#ifndef MLISTS_H__
#define MLISTS_H__

// This is the core of the library, include this header file to include the
// generic macro lists to a project, check mlists_config.h, if you want to add
// a feature, mlists_config.h comes configured already to give the intended
// functionality of the library without any unneeded and boilerplate features

/*
TODO:
() gnu typeof iterators for other than array?
() mlists_gfunctions.h and .c
*/

// Configuration header, first included in container.h
// Just to make clear that something depends on it here
#include "mlists_config.h"

#include "array.h"
#include "queue.h"
#include "stack.h"
#include "singly.h"
#include "doubly.h"

// Not implemented yet
#if defined MLISTS_WANT_GENERIC_FUNCTIONS
#	include "mlists_gfunctions.h"
#endif

#endif // MLISTS_H__

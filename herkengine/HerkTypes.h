// Copyright (C) 2011-2013, GReaperEx(Marios F.)
// This file is part of the "Herakles Engine" project
/* For conditions about distribution and use, consult
   the GNU GPLv3.                                      */

#ifndef __HERK_TYPES_H_INCLUDED__
#define __HERK_TYPES_H_INCLUDED__

#include "HerkCompileConfig.h"

namespace herk
{

//! This is a typedef for unsigned char, it gives a better name conversion
typedef unsigned char u8;
//! This is a typedef for signed char, it gives a better name conversion
typedef signed char   s8;

//! This is a typedef for char, it gives a better name conversion
typedef char c8;

//! This is a typedef for unsigned short, it gives a better name conversion
typedef unsigned short u16;
//! This is a typedef for signed short, it gives a better name conversion
typedef signed short   s16;

//! This is a typedef for unsigned int, it gives a better name conversion
typedef unsigned int u32;
//! This is a typedef for signed int, it gives a better name conversion
typedef signed int   s32;

//! This is a typedef for float, it gives a better name conversion
typedef float  f32;

//! This is a typedef for double, it gives a better name conversion
typedef double f64;

}

//! define a break macro for debugging
#include <cassert>

#define _HERK_DEBUG_BREAK_IF(_CONDITION_) assert(!(_CONDITION_))


#endif // __HERK_TYPES_H_INCLUDED__

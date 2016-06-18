// Copyright (C) 2011-2013, GReaperEx(Marios F.)
// This file is part of the "Herakles Engine" project
/* For conditions about distribution and use, consult
   the GNU GPLv3.                                      */

#ifndef __HERK_COMPILE_CONFIG_H_INCLUDED__
#define __HERK_COMPILE_CONFIG_H_INCLUDED__

//! Herakles Engine Version numbers
#define HERK_VERSION_MAJOR 0
#define HERK_VERSION_MINOR 0
#define HERK_VERSION_REVISION 16

/* This flag will be defined only in SVN, the official release code will
   have it undefined                                                     */
#define HERAKLES_VERSION_SVN
#define HERAKLES_SDK_VERSION "0.0.16"

#include <cstdio> // Needed for system defines

//! Currently, only SDL2 can make Herakles multi-platform
//! Define _HERK_COMPILE_FOR_SDL2_ if you want to use SDL2 as the platform of choice
#define _HERK_COMPILE_FOR_SDL2_

//! Fall-back device, for windows only
#ifndef _HERK_COMPILE_FOR_SDL2_
 #if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
  #define _HERK_COMPILE_FOR_WINDOWS
 #else
  #error "The fall-back device supports Windows only."
 #endif
#endif

//! Define _HERK_COMPILE_WITH_MD2_LOADER_ if you want to load Quake 2 animated files
#define _HERK_COMPILE_WITH_MD2_LOADER_

//! Define _HERK_COMPILE_WITH_VTX_LOADER_ if you want to load Vertex geometry files
#define _HERK_COMPILE_WITH_VTX_LOADER_


//! Define _HERK_COMPILE_WITH_BMP_LOADER_ if you want to load .bmp/.dib files
#define _HERK_COMPILE_WITH_BMP_LOADER_
//! Define _HERK_COMPILE_WITH_PCX_LOADER_ if you want to load .pcx files
#define _HERK_COMPILE_WITH_PCX_LOADER_
//! Define _HERK_COMPILE_WITH_TGA_LOADER_ if you want to load .tga files
#define _HERK_COMPILE_WITH_TGA_LOADER_

#if defined(_HERK_COMPILE_WITH_BMP_LOADER_) || \
    defined(_HERK_COMPILE_WITH_PCX_LOADER_) || \
    defined(_HERK_COMPILE_WITH_TGA_LOADER_)

#define _HERK_COMPILE_WITH_LOAD_CONVERTER

#endif

//! Define _HERK_COMPILE_WITH_BMP_WRITER_ if you want to write .bmp/.dib files
#define _HERK_COMPILE_WITH_BMP_WRITER_
//! Define _HERK_COMPILE_WITH_PCX_WRITER_ if you want to write .pcx files
#define _HERK_COMPILE_WITH_PCX_WRITER_
//! Define _HERK_COMPILE_WITH_TGA_WRITER_ if you want to write .tga files
#define _HERK_COMPILE_WITH_TGA_WRITER_

#if defined(_HERK_COMPILE_WITH_BMP_WRITER_) || \
    defined(_HERK_COMPILE_WITH_PCX_WRITER_) || \
    defined(_HERK_COMPILE_WITH_TGA_WRITER_)

#define _HERK_COMPILE_WITH_WRITE_CONVERTER

#endif

//! Define _HERK_DOUBLE_PRECISION if you want to build a mathematically more accurate engine
//#define _HERK_DOUBLE_PRECISION

// Herakles Engine is currently distributed as static library and source code
// This way, it is both flexible and easy to debug by users

#endif // __HERK_COMPILE_CONFIG_H_INCLUDED__

/* Copyright (C) 2012-2014, GReaperEx(Marios F.)
   This file is part of OverSight(OS)

   OverSight(OS) is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   OverSight(OS) is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with OverSight(OS).  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __OS_TYPES_H_
#define __OS_TYPES_H_

/* Converts escape characters like "\t", "\12" or "\xF0" to their one-byte equivalent */
void OSstrtostr(char* dst, const char* src); /* dst must be long enough to hold the value */

void OSstrtoint(int* dst, const char* src, int base);
void OSinttostr(char* dst, int src, int base); /* dst must be long enough to hold the value */

void OSstrtouint(unsigned int* dst, const char* src, int base);
void OSuinttostr(char* dst, unsigned int src, int base);  /* dst must be long enough to hold the value */

void OSstrtof(float* dst, const char* src);
/* before_point: Number of digits left of the decimal point
   after_point: Number of digits right of the decimal point ( no point if zero )
   scientific: boolean, use scientific (1) or fixed point (0) notation           */
void OSftostr(char* dst, float src, int before_point, int after_point, int scientific);

void OSstrtod(double* dst, const char* src);
/* same as float */
void OSdtostr(char* dst, double src, int before_point, int after_point, int scientific);

void OSstrtold(long double* dst, const char* src);
/* same as float */
void OSldtostr(char* dst, long double src, int before_point, int after_point, int scientific);

/* Typedefs for clarity */

typedef unsigned char uint8_t;
typedef   signed char  int8_t;

typedef unsigned short uint16_t;
typedef   signed short  int16_t;

typedef unsigned int uint32_t;
typedef   signed int  int32_t;

/*
typedef unsigned long uint64_t;
typedef   signed long  int64_t;
*/

typedef enum { FALSE = 0, TRUE = 1 }BOOL;

typedef unsigned int size_t;

#endif /* __OS_TYPES_H_ */


/* Copyright (C) 2012-2013, GReaperEx(Marios F.)
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

#ifndef __SSE_H_
#define __SSE_H_

typedef struct
{
    unsigned int FPU    : 1;
    unsigned int FXSR   : 1;
    unsigned int MMX    : 1;

    unsigned int SSE    : 1;
    unsigned int SSE2   : 1;
    unsigned int SSE3   : 1;
    unsigned int SSSE3  : 1;
    unsigned int SSE4_1 : 1;
    unsigned int SSE4_2 : 1;
    unsigned int SSE4A  : 1;

    unsigned int XOP    : 1;
    unsigned int FMA4   : 1;
    unsigned int CVT16  : 1;
    unsigned int AVX    : 1;
}FPU_SSE_bitfield;

FPU_SSE_bitfield GetFPUnSSEsupportLevel(void);

/* Also initializes the FPU */
int EnableSSE(void);

#endif


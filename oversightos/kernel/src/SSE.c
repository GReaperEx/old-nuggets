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

#include "SSE.h"

FPU_SSE_bitfield GetFPUnSSEsupportLevel(void)
{
    FPU_SSE_bitfield bfield;

    unsigned int ECX, EDX;

    __asm__ __volatile__(
       "mov $0x1, %%eax \n\t"
       "cpuid \n\t"
       "mov %%ecx, %0 \n\t"
       "mov %%edx, %1 \n\t"
     : "=m"(ECX), "=m"(EDX)
     :
     : "%eax", "%ebx", "%ecx", "%edx"
    );

    bfield.FPU = EDX & 0x1;
    bfield.FXSR = (EDX >> 24) & 0x1;
    bfield.MMX = (EDX >> 23) & 0x1;

    bfield.SSE = (EDX >> 25) & 0x1;
    bfield.SSE2 = (EDX >> 26) & 0x1;
    bfield.SSE3 = ECX & 0x1;
    bfield.SSSE3 = (ECX >> 9) & 0x1;
    bfield.SSE4_1 = (ECX >> 19) & 0x1;
    bfield.SSE4_2 = (ECX >> 20) & 0x1;
    bfield.SSE4A = (ECX >> 6) & 0x1;

    bfield.XOP = (ECX >> 11) & 0x1;
    bfield.FMA4 = (ECX >> 16) & 0x1;
    bfield.CVT16 = (ECX >> 29) & 0x1;
    bfield.AVX = (ECX >> 28) & 0x1;

    return bfield;
}

int EnableSSE(void)
{
    FPU_SSE_bitfield bfield;

    bfield = GetFPUnSSEsupportLevel();

    /* Set the FPU to default values */
    __asm__ __volatile__("fninit \n\t");

    if (!bfield.SSE) /* Abort if SSE not supported */
        return 0;

    /* Enabling SSE */
    __asm__ __volatile__(
       "mov %%cr0, %%eax \n\t"
       "and $0xFFFB, %%ax \n\t"
       "or $0x2, %%ax \n\t"
       "mov %%eax, %%cr0 \n\t"
       "mov %%cr4, %%eax \n\t"
       "or $0x600, %%eax \n\t"
       "mov %%eax, %%cr4 \n\t"
    ::
     : "%eax"
    );

    return 1;
}


/* Copyright (C) 2014, GReaperEx(Marios F.)
   This file is part of OverSight(OS)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** This provides a library for 64-bit integer emulation on 32-bit systems */

#ifndef __KINT64_H_
#define __KINT64_H_

#include "OStypes.h"

typedef struct
{
    uint32_t low;
    uint32_t high;
}kInt64;

void kInt64_fromStr(kInt64* dst, const char* src, int base, BOOL _signed);
void kInt64_toStr(char* dst, const kInt64* src, int base, BOOL _signed);

/** All these work like CPU instructions( without flags though ) */

void kInt64_ADD(kInt64* dst, const kInt64* src);
void kInt64_INC(kInt64* dst);
void kInt64_SUB(kInt64* dst, const kInt64* src);
void kInt64_DEC(kInt64* dst);
void kInt64_MUL(kInt64* dst, const kInt64* src);
void kInt64_DIV(kInt64* dstDiv, kInt64* dstRem, const kInt64* src);
void kInt64_iMUL(kInt64* dst, const kInt64* src); /** For signed values */
void kInt64_iDIV(kInt64* dstDiv, kInt64* dstRem, const kInt64* src); /** For signed values */

void kInt64_NOT(kInt64* dst);
void kInt64_AND(kInt64* dst, const kInt64* src);
void kInt64_OR(kInt64* dst, const kInt64* src);
void kInt64_XOR(kInt64* dst, const kInt64* src);

void kInt64_SHL(kInt64* dst, int places);
void kInt64_SHR(kInt64* dst, int places);

/* Returns -1 for "src1 < src2", 0 for "src1 == src2" and 1 for "src1 > src2" */
int kInt64_CMP(const kInt64* src1, const kInt64* src2);
int kInt64_iCMP(const kInt64* src1, const kInt64* src2); /** For signed values */

void kInt64_ClrBit(kInt64* dst, int index);
void kInt64_SetBit(kInt64* dst, int index);
BOOL  kInt64_GetBit(const kInt64* src, int index);



#endif /* __KINT64_H_ */

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

#include "kInt64.h"

void kInt64_fromStr(kInt64* dst, const char* src, int base, BOOL _signed)
{
    char c;
    kInt64 toAdd, toMult;
    BOOL neg = FALSE;

    if (_signed)
    {
        if ((c = *src) == '+')
            ++src;
        else if (c == '-')
        {
            ++src;
            neg = TRUE;
        }
    }

    if (base >= 2 && base <= 16)
    {
        dst->low = dst->high = 0;
        toAdd.low = toAdd.high = 0;
        toMult.low = base;
        toMult.high = 0;

        while ((c = *src++) != '\0')
        {
            kInt64_MUL(dst, &toMult);

            if (c >= '0' && c < '0' + base)
                toAdd.low = c - '0';
            else if (base > 10)
            {
                if (c >= 'A' && c < 'A' + (base - 10))
                    toAdd.low = (c - 'A') + 10;
                else if (c >= 'a' && c < 'a' + (base - 10))
                    toAdd.low = (c - 'a') + 10;
                else
                    break;
            }

            kInt64_ADD(dst, &toAdd);
        }

        if (neg)
        {
            kInt64 zero = { 0, 0 };

            kInt64_SUB(&zero, dst);
            *dst = zero;
        }
    }
}

void kInt64_toStr(char* dst, const kInt64* src, int base, BOOL _signed)
{
    static const char digits[ ] = "0123456789ABCDEF";
    static char tempStr[65];
    int i, digitCount = 0;
    kInt64 toDiv, tempInt;
    kInt64 rem;
    BOOL neg = FALSE;

    tempInt = *src;

    if (_signed)
    {
        if (kInt64_GetBit(&tempInt, 63))
        {
            kInt64 zero = { 0, 0 };

            neg = TRUE;

            kInt64_SUB(&zero, &tempInt);
            tempInt = zero;
        }
    }

    if (base >= 2 && base <= 16)
    {
        toDiv.low = base;
        toDiv.high = 0;

        do
        {
            kInt64_DIV(&tempInt, &rem, &toDiv);
            tempStr[digitCount++] = digits[rem.low];
        }while ((tempInt.low != 0 || tempInt.high != 0));

        /* Inverting the string */
        if (neg)
            *dst++ = '-';

        dst[digitCount] = '\0';
        for (i = digitCount - 1; i >= 0; i--){
            dst[digitCount - 1 - i] = tempStr[i];
        }
    }
}

void kInt64_ADD(kInt64* dst, const kInt64* src)
{
    __asm__ __volatile__(
       "movl %2, %%eax \n\t"
       "movl %3, %%edx \n\t"
       "addl %%eax, %0 \n\t"
       "adcl %%edx, %1 \n\t"
     : "=m"(dst->low), "=m"(dst->high)
     : "m"(src->low), "m"(src->high)
     : "%eax", "%edx"
    );
}

void kInt64_INC(kInt64* dst)
{
    __asm__ __volatile__(
       "movl $0x1, %%eax \n\t"
       "xorl %%edx, %%edx \n\t"
       "addl %%eax, %0 \n\t"
       "adcl %%edx, %1 \n\t"
     : "=m"(dst->low), "=m"(dst->high)
     :
     : "%eax", "%edx"
    );
}

void kInt64_SUB(kInt64* dst, const kInt64* src)
{
    __asm__ __volatile__(
       "movl %2, %%eax \n\t"
       "movl %3, %%edx \n\t"
       "subl %%eax, %0 \n\t"
       "sbbl %%edx, %1 \n\t"
     : "=m"(dst->low), "=m"(dst->high)
     : "m"(src->low), "m"(src->high)
     : "%eax", "%edx"
    );
}

void kInt64_DEC(kInt64* dst)
{
    __asm__ __volatile__(
       "movl $0x1, %%eax \n\t"
       "xorl %%edx, %%edx \n\t"
       "subl %%eax, %0 \n\t"
       "sbbl %%edx, %1 \n\t"
     : "=m"(dst->low), "=m"(dst->high)
     :
     : "%eax", "%edx"
    );
}

void kInt64_MUL(kInt64* dst, const kInt64* src)
{
    kInt64 result;

    __asm__ __volatile__(
       "movl %2, %%eax \n\t"
       "mull %4 \n\t"
       "movl %%eax, %0 \n\t"
       "movl %%edx, %1 \n\t"
       "movl %3, %%eax \n\t"
       "mull %4 \n\t"
       "addl %%eax, %1 \n\t"
       "movl %2, %%eax \n\t"
       "mull %5 \n\t"
       "addl %%eax, %1 \n\t"
     : "=m"(result.low), "=m"(result.high)
     : "m"(dst->low), "m"(dst->high), "m"(src->low), "m"(src->high)
     : "%eax", "%edx"
    );

    *dst = result;
}

void kInt64_DIV(kInt64* dstDiv, kInt64* dstRem, const kInt64* src)
{
    int i;
    kInt64 temp, tempBit;

    if (src->low == 0 && src->high == 0)
        return;

    temp = *dstDiv;
    dstDiv->low = dstDiv->high = 0;
    dstRem->low = dstRem->high = 0;
    tempBit.low = tempBit.high = 0;

    for (i = 63; i >= 0; i--){
        tempBit.low = kInt64_GetBit(&temp, i);
        kInt64_SHL(dstRem, 1);
        kInt64_OR(dstRem, &tempBit);
        kInt64_SHL(dstDiv, 1);

        while (kInt64_CMP(dstRem, src) >= 0)
        {
            kInt64_INC(dstDiv);
            kInt64_SUB(dstRem, src);
        }
    }
}

void kInt64_iMUL(kInt64* dst, const kInt64* src)
{
    BOOL neg = FALSE;

    kInt64 tempDst = { 0, 0 }, tempSrc = { 0, 0 };

    if (kInt64_GetBit(dst, 63))
    {
        neg = !neg;
        kInt64_SUB(&tempDst, dst);
    }
    else
        tempDst = *dst;

    if (kInt64_GetBit(src, 63))
    {
        neg = !neg;
        kInt64_SUB(&tempSrc, src);
    }
    else
        tempSrc = *src;

    kInt64_MUL(&tempDst, &tempSrc);

    if (neg)
    {
        dst->low = dst->high = 0;
        kInt64_SUB(dst, &tempDst);
    }
    else
        *dst = tempDst;
}

void kInt64_iDIV(kInt64* dstDiv, kInt64* dstRem, const kInt64* src)
{
    BOOL neg = FALSE;

    kInt64 tempDst = { 0, 0 }, tempSrc = { 0, 0 };

    if (kInt64_GetBit(dstDiv, 63))
    {
        neg = !neg;
        kInt64_SUB(&tempDst, dstDiv);
    }
    else
        tempDst = *dstDiv;

    if (kInt64_GetBit(src, 63))
    {
        neg = !neg;
        kInt64_SUB(&tempSrc, src);
    }
    else
        tempSrc = *src;

    kInt64_DIV(&tempDst, dstRem, &tempSrc);

    if (neg)
    {
        dstDiv->low = dstDiv->high = 0;
        kInt64_SUB(dstDiv, &tempDst);
    }
    else
        *dstDiv = tempDst;
}

void kInt64_NOT(kInt64* dst)
{
    dst->low = ~dst->low;
    dst->high = ~dst->high;
}

void kInt64_AND(kInt64* dst, const kInt64* src)
{
    dst->low &= src->low;
    dst->high &= src->high;
}

void kInt64_OR(kInt64* dst, const kInt64* src)
{
    dst->low |= src->low;
    dst->high |= src->high;
}

void kInt64_XOR(kInt64* dst, const kInt64* src)
{
    dst->low ^= src->low;
    dst->high ^= src->high;
}

void kInt64_SHL(kInt64* dst, int places)
{
    if (places > 32)
        dst->high = dst->low << (places - 32);
    else
        dst->high = (dst->high << places) | (dst->low >> (32 - places));

    dst->low <<= places;
}

void kInt64_SHR(kInt64* dst, int places)
{
    if (places > 32)
        dst->low = dst->high >> (places - 32);
    else
        dst->low = (dst->low >> places) | (dst->high << (32 - places));

    dst->high >>= places;
}

int kInt64_CMP(const kInt64* src1, const kInt64* src2)
{
    if (src1->high > src2->high)
        return 1;

    if (src1->high < src2->high)
        return -1;

    if (src1->low > src2->low)
        return 1;

    if (src1->low < src2->low)
        return -1;

    return 0;
}

int kInt64_iCMP(const kInt64* src1, const kInt64* src2)
{
    kInt64 temp1, temp2;
    int neg1, neg2;

    temp1.high = temp1.low = 0;
    temp2.high = temp2.low = 0;
    /* Getting the signs */
    neg1 = src1->high >> 31;
    neg2 = src2->high >> 31;

    if (!neg1 && neg2)
        return 1;

    if (neg1 && !neg2)
        return -1;

    if (neg1)
    {
        kInt64_SUB(&temp1, src1);
        kInt64_SUB(&temp2, src2);
    }
    else
    {
        temp1 = *src1;
        temp2 = *src2;
    }

    return kInt64_CMP(&temp1, &temp2);
}

void kInt64_ClrBit(kInt64* dst, int index)
{
    kInt64 temp = { 1, 0 };

    kInt64_SHL(&temp, index);
    kInt64_NOT(&temp);
    kInt64_AND(dst, &temp);
}

void kInt64_SetBit(kInt64* dst, int index)
{
    kInt64 temp = { 1, 0 };

    kInt64_SHL(&temp, index);
    kInt64_OR(dst, &temp);
}

BOOL kInt64_GetBit(const kInt64* src, int index)
{
    if (index >= 32)
        return (src->high >> (index - 32)) & 0x1;

    return (src->low >> index) & 0x1;
}

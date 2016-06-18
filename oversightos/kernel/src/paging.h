/* Copyright (C) 2014, GReaperEx(Marios F.)
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

#ifndef __PAGING_H_
#define __PAGING_H_

#include "OStypes.h"

#ifndef NULL
 #define NULL ((void*)0)
#endif

#define PDT_ADDRESS_MASK 0xFFFFF000

typedef union
{
    uint32_t lvalue;

    struct
    {
        uint32_t present       : 1;
        uint32_t readWrite     : 1;
        uint32_t userSuper     : 1;
        uint32_t writeThrough  : 1;
        uint32_t cacheDisabled : 1;
        uint32_t accessed      : 1;
        uint32_t _res1         : 1;
        uint32_t pageSize      : 1;
        uint32_t _res2         : 1;
        uint32_t avail         : 3;

        uint32_t ptAddress     : 20;
    };
}PD_entry;

typedef union
{
    uint32_t lvalue;

    struct
    {
        uint32_t present       : 1;
        uint32_t readWrite     : 1;
        uint32_t userSuper     : 1;
        uint32_t writeThrough  : 1;
        uint32_t cacheDisabled : 1;
        uint32_t accessed      : 1;
        uint32_t dirty         : 1;
        uint32_t _res1         : 1;
        uint32_t global        : 1;
        uint32_t avail         : 3;

        uint32_t ppAddress     : 20;
    };
}PT_entry;

/* Assuming the last PD_entry points to the PDT */

static inline PD_entry* getPD_entry(void* vAddress)
{
    uint32_t vAddr = (uint32_t)vAddress;

    return ((PD_entry*)0xFFFFF000) + (vAddr >> 22);
}

/* Assumes that the requested PT_entry exists */
static inline PT_entry* getPT_entry(void* vAddress)
{
    uint32_t vAddr = (uint32_t)vAddress;

    return ((PT_entry*)0xFFC00000) + (vAddr >> 12);
}

static inline void setPDTable(void* pAddress)
{
    __asm__ __volatile__(
       "movl    %0, %%eax \n\t"
       "movl %%eax, %%cr3 \n\t"
    :: "m"(pAddress)
     : "%eax"
    );
}

static inline void invalidate_page(void* address)
{
    __asm__ __volatile__("invlpg (%0)" ::"r" (address) : "memory");
}

static inline BOOL isLegitAddress(const void* address)
{
    uint32_t ptrValue = (uint32_t)address;

    return ptrValue >= (0x100000 + 4096) && ptrValue < 0xC0000000;
}

void paging_enable(void* pAddress);

/* Copies the current PDT's first and >=3GB entries, except
   the final one, which points to the table itself          */
BOOL paging_setupPDT(PD_entry PDT[]);

/* Setups the alloc/free interface. Call it *after* paging is enabled, only once */
void paging_initAlloc(void);

/* Allocates a single 4KB page and returns its address, and NULL on failure:
        dstVirtual:
            if NULL: If isSuper is true, return an address above 3GB, else above 1MB
            else   : Map srcAddress on dstVirtual and return dstVirtual
        srcAddress:
            if NULL: Allocate a "random" physical page
            else   :
                if isSrcVirtual is true, remap the entry's physical address
                    If entry doesn't exist, use a new physical page
                else, map that address as physical on the requested entry
        freeDstPhysical:
            If true and dstVirtual is overwritten, the previous physical page is released */
void* paging_alloc(void* dstVirtual, BOOL freeDstPhysical,
                   void* srcAddress, BOOL isSrcVirtual,
                   BOOL isSuper, BOOL isWritable);

/* Frees virtual page, if freePhysical is true then the physical page is also released */
void paging_free(void* srcVirtual, BOOL freePhysical);

/* Frees physical pages of completely empty page entry tables */
void paging_trim(void* srcAddress, size_t pageAmount);

/* Returns whether there are enough free, adjacent virtual pages after baseAddress */
BOOL paging_isThereSpace(void* baseAddress, size_t pageAmount);

size_t OS_totalPages(void); /* Returns total amount of allocatable pages */
size_t OS_usedPages(void);  /* Returns used(allocated) amount of pages   */

#endif /* __PAGING_H_ */

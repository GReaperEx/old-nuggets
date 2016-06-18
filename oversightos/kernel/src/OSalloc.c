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

#include "OSalloc.h"

#include "mmapInfo.h"
#include "paging.h"

void* OSalloc(size_t pageAmount, BOOL kernelSpace)
{
    void* allocAddress = NULL;
    size_t lowLim = 0x100000;
    size_t highLim = 0xC0000000;

    if (kernelSpace)
    {
        lowLim  = 0xC0000000;
        highLim = 0xFFC00000;
    }

    if (pageAmount > 0)
    {
        size_t usedPages;
        size_t totalPages;

        usedPages = OS_usedPages();
        totalPages = OS_totalPages();

        /* Checking if there are enough physical pages for this, including
           extra allocations due to page directory table expansion         */
        if (totalPages - usedPages >= pageAmount + (pageAmount + 1023)/1024)
        {
            uint32_t i;

            /* Searching for empty space */
            for (i = lowLim; i < highLim; i+=4*1024) {
                if (paging_isThereSpace((void*)i, pageAmount))
                    break;
            }

            if (i < highLim)
            {
                int j;

                allocAddress = (void*)i;
                /* Allocating the page array */
                for (j = 0; j < pageAmount; j++, i+=4*1024) {
                    /* It really can't fail right now, therefore it's sane to ignore the returned address */
                    paging_alloc((void*)i, FALSE, NULL, FALSE, kernelSpace, TRUE);
                }
            }
        }
    }

    return allocAddress;
}

void* OSrealloc(void* oldptr, size_t oldAmount, size_t newAmount, BOOL kernelSpace)
{
    size_t lowLim = 0x100000;
    size_t highLim = 0xC0000000;

    if (kernelSpace)
    {
        lowLim  = 0xC0000000;
        highLim = 0xFFC00000;
    }

    if (!newAmount)
    {
        OSfree(oldptr, oldAmount);
        return NULL;
    }

    if (!oldptr || !oldAmount)
        return OSalloc(newAmount, kernelSpace);

    if (oldAmount > newAmount)
        OSfree( (void*)((uint32_t)oldptr + newAmount*4096), oldAmount - newAmount );
    else
    {
        uint32_t baseAddr;
        size_t usedPages;
        size_t totalPages;
        size_t remAmount;

        baseAddr = (uint32_t)oldptr + oldAmount*4096;
        usedPages = OS_usedPages();
        totalPages = OS_totalPages();
        remAmount = newAmount - oldAmount;

        if (totalPages - usedPages >= remAmount + (remAmount + 1023)/1024)
        {
            if (paging_isThereSpace((void*)baseAddr, remAmount))
            {
                int i;

                for (i = 0; i < remAmount; i++, baseAddr+=4096) {
                    (void) paging_alloc((void*)baseAddr, FALSE, NULL, FALSE, kernelSpace, TRUE);
                }
            }
            else
            {
                uint32_t i;
                int j;

                /* Searching for empty space in the local area */
                if (paging_isThereSpace( (void*)((uint32_t)oldptr - 4096*remAmount), remAmount ))
                    i = (uint32_t)oldptr - 4096*remAmount;
                else
                {
                    /* Searching for empty space "worldwide" */
                    for (i = lowLim; i < highLim; i+=4096) {
                        if (paging_isThereSpace((void*)i, newAmount))
                            break;
                    }
                }

                if (i < highLim)
                {
                    uint32_t oldAddr = (uint32_t)oldptr;

                    oldptr = (void*)i;
                    /* Allocating the page array, remapping previous ones */
                    for (j = 0; j < oldAmount; j++, i+=4*1024, oldAddr+=4*1024) {
                        /* It really can't fail right now, therefore it's sane to ignore the returned address */
                        (void) paging_alloc((void*)i, FALSE, (void*)oldAddr, TRUE, kernelSpace, TRUE);

                        paging_free((void*)oldAddr, FALSE);
                    }
                    paging_trim((void*)(oldAddr - 4096*oldAmount), oldAmount);

                    /* Allocating the rest of the page array, with new physical memory */
                    for (; j < newAmount; j++, i+=4*1024) {
                        /* It really can't fail right now, therefore it's sane to ignore the returned address */
                        (void) paging_alloc((void*)i, FALSE, NULL, FALSE, kernelSpace, TRUE);
                    }
                }
                else
                    oldptr = NULL;
            }
        }
        else
            oldptr = NULL;
    }

    return oldptr;
}

void OSfree(void* ptr, size_t amount)
{
    uint32_t i, j;

    if (ptr)
    {
        i = (uint32_t)ptr;
        for (j = 0; j < amount; j++, i+=4*1024) {
            paging_free((void*)i, TRUE);
        }
        paging_trim(ptr, amount);
    }
}

void OSfreeAll(void)
{
    OSfree( (void*)0x100000, (0xC0000000 - 0x100000)/4096 );
}


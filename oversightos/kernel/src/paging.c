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

#include "paging.h"

#include "mmapInfo.h"

/* Every bit represents a 4-KByte page ( physical space ) */
volatile uint8_t MemoryBitmap[131072];

volatile size_t totalMemPages;
volatile size_t usedMemPages;
volatile uint32_t maxAddress;


static inline void _alloc_page(uint32_t offset)
{
    MemoryBitmap[offset >> 15] |= 0x1 << ((offset >> 12) & 0x7);
    ++usedMemPages;
}

static inline void _free_page(uint32_t offset)
{
    MemoryBitmap[offset >> 15] &= ~(0x1 << ((offset >> 12) & 0x7));
    --usedMemPages;
}

void* first_avail_page(void);

void paging_enable(void* pAddress)
{
    setPDTable(pAddress);

    __asm__ __volatile__(
       /* Activating paging */
       "movl %%cr0, %%eax \n\t"
       "orl $0x80000000, %%eax \n\t"
       "movl %%eax, %%cr0 \n\t"
    ::
     : "%eax"
    );
}

BOOL paging_setupPDT(PD_entry PDT[])
{
    PD_entry* pdtPtr;
    PD_entry* lowMB;
    int i;

    pdtPtr = getPD_entry((void*)0);

    lowMB = paging_alloc(NULL, FALSE, NULL, FALSE, TRUE, TRUE);

    if (lowMB)
    {
        /* Identity map the 1st MByte */
        for (i = 0; i < 256; i++) {
            lowMB[i].lvalue = 0;
            lowMB[i].present = 1;
            lowMB[i].readWrite = 1;
            lowMB[i].userSuper = 1;
            lowMB[i].ptAddress = (i*4096) >> 12;
        }
        for (; i < 1024; i++) {
            lowMB[i].lvalue = 0;
        }
        /* Remapping the page */
        PDT[0].lvalue = 0;
        PDT[0].present = 1;
        PDT[0].readWrite = 1;
        PDT[0].userSuper = 1;
        PDT[0].ptAddress = getPT_entry(lowMB)->ppAddress;

        for (i = 1; i < 768; i++) {
            PDT[i].lvalue = 0;
        }
        /* Copy the high GByte */
        for (i = 768; i < 1023; i++) {
            PDT[i] = pdtPtr[i];
        }

        /* Last page table entry points to the PDT */
        PDT[1023].lvalue = 0;
        PDT[1023].present = 1;
        PDT[1023].readWrite = 1;
        PDT[1023].userSuper = 1;
        PDT[1023].ptAddress = getPT_entry(PDT)->ppAddress;

        return TRUE;
    }

    return FALSE;
}

void paging_initAlloc(void)
{
    uint32_t i, j;
    PD_entry *pdtAddress;
    PT_entry *petAddress;

    totalMemPages = 256;
    usedMemPages = 256;
    /* Finding total RAM capacity, may be slow */
    for (i = 1024*1024; i != 0; i += 4*1024) {
        if (IsMemoryUsable(i, 4*1024))
        {
            ++totalMemPages;
            maxAddress = i;
        }
    }

    /* Copying the kernel's information */
    pdtAddress = getPD_entry((void*)0);
    for (i = 768; i < 1023; i++) {
        if (pdtAddress[i].present)
        {
            petAddress = getPT_entry( (void*)(i << 22) );
            for (j = 0; j < 1024; j++){
                if (petAddress[j].present)
                    _alloc_page(petAddress[j].ppAddress << 12);
            }
        }
    }
}

void* findVirtualSpace(void* lowLimit, void* highLimit);

void* paging_alloc(void* dstVirtual, BOOL freeDstPhysical,
                   void* srcAddress, BOOL isSrcVirtual,
                   BOOL isSuper, BOOL isWritable)
{
    PT_entry* PTptr;

    /* Finding a new virtual address if none is provided */
    if (!dstVirtual)
    {
        if (isSuper)
            dstVirtual = findVirtualSpace((void*)0xC0000000, (void*)0xFFC00000);
        else
            dstVirtual = findVirtualSpace((void*)0x100000, (void*)0xC0000000);

        /* We're out of virtual memory, can't do anything but abort */
        if (!dstVirtual)
            return NULL;
    }

    /* Checking if a page entry table exists, creates it if not */
    if (!getPD_entry(dstVirtual)->present)
    {
        void* PET_physical;
        PD_entry* PDptr;
        int i;

        PET_physical = first_avail_page();
        if (!PET_physical)
            return NULL;

        PDptr = getPD_entry(dstVirtual);
        PDptr->lvalue = 0;
        PDptr->ptAddress = (uint32_t)PET_physical >> 12;
        PDptr->present = 1;
        PDptr->userSuper = 1;
        PDptr->readWrite = 1;

        invalidate_page((void*)((uint32_t)PDptr << 10));

        PTptr = (PT_entry*)((uint32_t)PDptr << 10);
        for (i = 0; i < 1024; i++) {
            PTptr[i].lvalue = 0;
        }
    }

    if (!srcAddress)
    {
        srcAddress = first_avail_page();
        if (!srcAddress)
        {
            /* Out of Memory, currently just aborts */
            return NULL;
        }
    }
    else
    {
        if (isSrcVirtual)
        {
            if (getPD_entry(srcAddress)->present && getPT_entry(srcAddress)->present)
                srcAddress = (void*)((uint32_t)(getPT_entry(srcAddress)->ppAddress) << 12);
            else
            {
                srcAddress = first_avail_page();
                if (!srcAddress)
                {
                    /* Out of Memory, currently just aborts */
                    return NULL;
                }
            }
        }
    }

    if (getPT_entry(dstVirtual)->present && freeDstPhysical)
        _free_page((uint32_t)(getPT_entry(dstVirtual)->ppAddress) << 12);

    PTptr = getPT_entry(dstVirtual);

    PTptr->lvalue = 0;
    PTptr->present = 1;
    PTptr->readWrite = isWritable;
    PTptr->userSuper = !isSuper;
    PTptr->ppAddress = (uint32_t)(srcAddress) >> 12;

    invalidate_page(dstVirtual);

    return dstVirtual;
}

void paging_free(void* srcVirtual, BOOL freePhysical)
{
    PD_entry* PDptr;
    PT_entry* PTptr;

    if (srcVirtual)
    {
        PDptr = getPD_entry(srcVirtual);
        if (PDptr->present)
        {
            PTptr = getPT_entry(srcVirtual);
            if (PTptr->present && freePhysical)
                _free_page((uint32_t)(PTptr->ppAddress) << 12);

            PTptr->lvalue = 0;
            invalidate_page(srcVirtual);
        }
    }
}

void paging_trim(void* srcAddress, size_t pageAmount)
{
    PD_entry *PDptr;
    PT_entry* PTptr;
    size_t i, j;
    size_t base, limit;

    base = (size_t)srcAddress;
    limit = base + pageAmount*4096;

    PDptr = getPD_entry(srcAddress);
    for (j = base; j < limit; j+=4*1024*1024, PDptr++) {
        PTptr = (PT_entry*)((uint32_t)PDptr << 10);

        for (i = 0; i < 1024; i++) {
            if (PTptr[i].lvalue != 0)
                break;
        }

        if (i == 1024)
        {
            _free_page((uint32_t)(PDptr->ptAddress) << 12);
            PDptr->lvalue = 0;
            invalidate_page(getPT_entry(srcAddress));
        }
    }
}

BOOL paging_isThereSpace(void* baseAddress, size_t pageAmount)
{
    uint32_t baseAddr;
    size_t i;

    baseAddr = (uint32_t)baseAddress & 0xFFFFF000;

    i = 0;
    while (i < pageAmount)
    {
        if (!(getPD_entry((void*)baseAddr)->present))
        {
            i += 1024;
            baseAddr += 4*1024*1024;
        }
        else if (!(getPT_entry((void*)baseAddr)->present))
        {
            i += 1;
            baseAddr += 4*1024;
        }
        else
            break;
    }

    return i >= pageAmount;
}

size_t OS_totalPages(void)
{
    return totalMemPages;
}

size_t OS_usedPages(void)
{
    return usedMemPages;
}


void* first_avail_page(void)
{
    static uint32_t i = 0;

    i += 4*1024;

    if (i < 1024*1024 || i > maxAddress)
        i = 1024*1024;

    if (usedMemPages < totalMemPages)
    {
        for (; i != 0 && i <= maxAddress; i += 4*1024) {
            if (!(MemoryBitmap[i >> 15] & (0x1 << ((i >> 12) & 0x7))) && IsMemoryUsable(i, 4*1024))
            {
                _alloc_page(i);

                return (void*)i;
            }
        }
    }

    return NULL;
}

void* findVirtualSpace(void* lowLimit, void* highLimit)
{
    uint32_t lowAddress = (uint32_t)lowLimit & 0xFFFFF000;
    uint32_t highAddress = (uint32_t)highLimit & 0xFFFFF000;

    while (lowAddress < highAddress)
    {
        if (!(getPD_entry((void*)lowAddress)->present) ||
             (getPD_entry((void*)lowAddress)->present && !(getPT_entry((void*)lowAddress)->present)) )
        {
            /* found available space */
            break;
        }

        lowAddress += 4*1024;
    }

    if (lowAddress < highAddress)
        return (void*)lowAddress;

    return NULL;
}


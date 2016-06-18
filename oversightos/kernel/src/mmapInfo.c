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

#include "mmapInfo.h"
#include "kInt64.h"

inline unsigned char HACK_PEEKb(void* addr)
{
    unsigned char value;

    __asm__ __volatile__("mov           %1, %%eax \n\t"
                         "mov %%es:(%%eax), %%al  \n\t"
                         "mov         %%al, %0    \n\t"
                        : "=m"(value) : "m"(addr) : "%eax");
    return value;
}

inline unsigned short HACK_PEEKw(void* addr)
{
    unsigned short value;

    __asm__ __volatile__("mov           %1, %%eax \n\t"
                         "mov %%es:(%%eax), %%ax  \n\t"
                         "mov         %%ax, %0    \n\t"
                        : "=m"(value) : "m"(addr) : "%eax");
    return value;
}

inline unsigned int HACK_PEEKd(void* addr)
{
    unsigned int value;

    __asm__ __volatile__("mov           %1, %%eax \n\t"
                         "mov %%es:(%%eax), %%eax \n\t"
                         "mov        %%eax, %0    \n\t"
                        : "=m"(value) : "m"(addr) : "%eax");
    return value;
}


typedef struct
{
    kInt64 base;
    kInt64 size;

    unsigned int type;
}mmap_entry;

mmap_entry MemoryMap[MAX_MMAP_SIZE];
int mmap_size;

void GatherMemMap(void* bootInfoAddr)
{
    MultibootInfo* bootInfo = bootInfoAddr;
    unsigned int i;
    int j;

    if (HACK_PEEKd(bootInfo) & 0x40)
    {
        for (i = 0, j = 0;
             i < HACK_PEEKd((void*)((unsigned long)bootInfo + 44));
             i += HACK_PEEKd((void*)(HACK_PEEKd((void*)((unsigned long)bootInfo + 48)) + i - 4))+4, j++)
        {
            MemoryMap[j].base.low = HACK_PEEKd((void*)(HACK_PEEKd((void*)((unsigned long)bootInfo + 48)) + i));
            MemoryMap[j].base.high = HACK_PEEKd((void*)(HACK_PEEKd((void*)((unsigned long)bootInfo + 48)) + i + 4));

            MemoryMap[j].size.low = HACK_PEEKd((void*)(HACK_PEEKd((void*)((unsigned long)bootInfo + 48)) + i + 8));
            MemoryMap[j].size.high = HACK_PEEKd((void*)(HACK_PEEKd((void*)((unsigned long)bootInfo + 48)) + i + 12));

            MemoryMap[j].type = HACK_PEEKd((void*)(HACK_PEEKd((void*)((unsigned long)bootInfo + 48)) + i + 16));
        }
        mmap_size = j;
    }
    else if (HACK_PEEKd(bootInfo) & 0x1)
    {
        MemoryMap[0].base.low = 0;
        MemoryMap[0].base.high = 0;
        MemoryMap[0].size.low = HACK_PEEKd((void*)((unsigned long)bootInfo + 4))*1024;
        MemoryMap[0].size.high = 0;
        MemoryMap[0].type = 1;

        MemoryMap[1].base.low = 0x100000;
        MemoryMap[1].base.high = 0;
        MemoryMap[1].size.low = HACK_PEEKd((void*)(bootInfo + 8))*1024;
        MemoryMap[1].size.high = 0;
        MemoryMap[1].type = 1;

        mmap_size = 2;
    }
}

int IsMemoryUsable(unsigned long physicalAddr, unsigned long size)
{
    int i;

    kInt64 test, address, block;

    address.low = physicalAddr & 0xFFFFFFFF;
    address.high = 0;
    block.low = size & 0xFFFFFFFF;
    block.high = 0;

    kInt64_ADD(&block, &address);

    for (i = 0; i < mmap_size; i++){
        if (MemoryMap[i].type == 1)
        {
            test = MemoryMap[i].base;

            kInt64_ADD(&test, &MemoryMap[i].size);

            if (kInt64_CMP(&address, &MemoryMap[i].base) >= 0 && kInt64_CMP(&address, &test) < 0)
            {
                if (kInt64_CMP(&block, &test) <= 0)
                    return 1;

                block = test;
                kInt64_SUB(&block, &address);

                /* Ignoring possible 64-bit values */
                return IsMemoryUsable(test.low, size-block.low);
            }
        }
    }

    return 0;
}



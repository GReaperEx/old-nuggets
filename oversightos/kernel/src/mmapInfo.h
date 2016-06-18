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

#ifndef __MMAP_INFO_H_
#define __MMAP_INFO_H_

#define MAX_MMAP_SIZE 128

typedef struct __attribute__((packed))
{
    unsigned int flags;

    unsigned int mem_lower;
    unsigned int mem_upper;

    unsigned int boot_device;

    unsigned int cmdline;

    unsigned int mods_count;
    unsigned int mods_addr;

    unsigned int syms[3];

    unsigned int mmap_length;
    unsigned int mmap_addr;

    unsigned int drives_length;
    unsigned int drives_addr;

    unsigned int config_table;

    unsigned int boot_loader_name;

    unsigned int apm_table;

    unsigned int vbe_control_info;
    unsigned int vbe_mode_info;
    unsigned short vbe_mode;
    unsigned short vbe_interface_seg;
    unsigned short vbe_interface_off;
    unsigned int vbe_interface_len;
}MultibootInfo;


/* Call this only once, even before paging is enabled */
void GatherMemMap(void* bootInfoAddr);

int IsMemoryUsable(unsigned long physicalAddr, unsigned long size);

#endif /* __MMAP_INFO_H_ */


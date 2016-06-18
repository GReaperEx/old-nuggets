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

#ifndef __PORTS_H_
#define __PORTS_H_

static inline unsigned char inb(unsigned short port)
{
    unsigned char input;

    __asm__ __volatile__(
        "mov   %1, %%dx \n\t"
        "inb %%dx, %%al \n\t"
        "mov %%al, %0   \n\t"
     :  "=m"(input)
     :  "m"(port)
     :  "%al", "%dx"
    );

    return input;
}

static inline unsigned short inw(unsigned short port)
{
    unsigned short input;

    __asm__ __volatile__(
        "mov   %1, %%dx \n\t"
        "inw %%dx, %%ax \n\t"
        "mov %%ax, %0   \n\t"
     :  "=m"(input)
     :  "m"(port)
     :  "%ax", "%dx"
    );

    return input;
}

static inline unsigned int ind(unsigned short port)
{
    unsigned int input;

    __asm__ __volatile__(
        "mov    %1, %%dx  \n\t"
        "inl  %%dx, %%eax \n\t"
        "mov %%eax, %0    \n\t"
     :  "=m"(input)
     :  "m"(port)
     :  "%eax", "%dx"
    );

    return input;
}


static inline void outb(unsigned short port, unsigned char output)
{
    __asm__ __volatile__(
        "mov    %0, %%dx \n\t"
        "mov    %1, %%al \n\t"
        "outb %%al, %%dx \n\t"
     :: "m"(port), "m"(output)
     :  "%al", "%dx"
    );
}

static inline void outw(unsigned short port, unsigned short output)
{
    __asm__ __volatile__(
        "mov    %0, %%dx \n\t"
        "mov    %1, %%ax \n\t"
        "outw %%ax, %%dx \n\t"
     :: "m"(port), "m"(output)
     :  "%ax", "%dx"
    );
}

static inline void outd(unsigned short port, unsigned int output)
{
    __asm__ __volatile__(
        "mov     %0, %%dx  \n\t"
        "mov     %1, %%eax \n\t"
        "outl %%eax, %%dx  \n\t"
     :: "m"(port), "m"(output)
     :  "%eax", "%dx"
    );
}

#endif /* __PORTS_H_ */


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

#include "IDT.h"
#include "ports.h"

struct __attribute__((packed))
{
    unsigned short lenght;
    unsigned int base;
}IDTR;

volatile struct __attribute__((packed)) __attribute__((aligned(2)))
{
    unsigned short offset_lower16;
    unsigned short selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short offset_upper16;
}IDTentries[256];

void PIC_remap(int offset1, int offset2);

void IDT_Init(void)
{
    IDTR.lenght = 4096 - 1;
    IDTR.base = (unsigned int)IDTentries;

    PIC_remap(32, 40);

    __asm__ __volatile__("lidt %0 \n\t" :: "m"(IDTR) );
}

void IDT_SetGate(int gate, funcPtr ptr, GateTypes type, int user, int storage)
{
    unsigned int fPtr = (unsigned int)ptr;

    if (gate >= 0 && gate < 256)
    {
        IDTentries[gate].offset_lower16 = fPtr & 0xFFFF;
        IDTentries[gate].selector = 0x08;
        IDTentries[gate].zero = 0;
        IDTentries[gate].offset_upper16 = fPtr >> 16;

        switch (type)
        {
            case GT_INT:
                IDTentries[gate].type_attr = 0x8E | ((storage & 0x1) << 4) | (user ? 0x60 : 0x00);
            break;
            case GT_TRAP:
                IDTentries[gate].type_attr = 0x8F | ((storage & 0x1) << 4) | (user ? 0x60 : 0x00);
            break;
            case GT_TASK:
                IDTentries[gate].type_attr = 0x85 | ((storage & 0x1) << 4) | (user ? 0x60 : 0x00);
            break;
        }
    }
}

#define PIC1		0x20
#define PIC2		0xA0
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define ICW1_ICW4	0x01
#define ICW1_SINGLE	0x02
#define ICW1_INTERVAL4	0x04
#define ICW1_LEVEL	0x08
#define ICW1_INIT	0x10

#define ICW4_8086	0x01
#define ICW4_AUTO	0x02
#define ICW4_BUF_SLAVE	0x08
#define ICW4_BUF_MASTER	0x0C
#define ICW4_SFNM	0x10

void IRQ_Enable(int line)
{
    unsigned short port;
    unsigned char value;

    if (line < 8)
        port = PIC1_DATA;
    else
    {
        port = PIC2_DATA;
        line -= 8;
    }

    value = inb(port) & ~(0x1 << line);
    outb(port, value);
}

void IRQ_Disable(int line)
{
    unsigned short port;
    unsigned char value;

    if (line < 8)
        port = PIC1_DATA;
    else
    {
        port = PIC2_DATA;
        line -= 8;
    }

    value = inb(port) | (0x1 << line);
    outb(port, value);
}

void io_wait(void)
{
    __asm__ __volatile__(
        "jmp 0f \n\t"
        "0: \n\t"
        "jmp 1f \n\t"
        "1: \n\t"
    );
}

void PIC_remap(int offset1, int offset2)
{
    unsigned char a1, a2;

    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
    io_wait();
    outb(PIC1_DATA, offset1);
    io_wait();
    outb(PIC2_DATA, offset2);
    io_wait();
    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}



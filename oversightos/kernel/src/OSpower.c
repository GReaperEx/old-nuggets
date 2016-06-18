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

#include "TextUI.h"
#include "ports.h"

static void _hang(void);

void reboot_kbrd(void);
void reboot_tfault(void);

void shutdown_APM(void);


void OSpower_shutdown(void)
{
    TextUI_puts("You can now safely turn off your PC...\n");
    shutdown_APM();
    _hang();
}

void OSpower_reboot(void)
{
    TextUI_puts("You can now safely restart your PC...\n");
    reboot_kbrd();
    reboot_tfault();
    _hang();
}


static void _hang(void)
{
    /* Disabling the NMI */
    outb(0x70, inb(0x70) | 0x80);

    __asm__ __volatile__(
       "cli \n\t"
       "hlt \n\t"
    );
}

void reboot_kbrd(void)
{
    unsigned char good = 0x02;

    while (good & 0x02)
        good = inb(0x64);

    outb(0x64, 0xFE);
}

void reboot_tfault(void)
{
    struct __attribute__((packed))
    {
        unsigned short lenght;
        unsigned int base;
    }fakeIDT = { 0, 0 };

    __asm__ __volatile__("lidt %0 \n\t":: "m"(fakeIDT));
    __asm__ __volatile__("int $0x80 \n\t");
}

void shutdown_APM(void)
{
    /* currently does nothing */
}



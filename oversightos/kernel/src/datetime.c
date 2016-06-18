/* Copyright (C) 2012-2015, GReaperEx(Marios F.)
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

#include "datetime.h"
#include "ports.h"

#define CURRENT_YEAR    2015    /* This should change each year! */

#define CMOS_ADDRESS    0x70
#define CMOS_DATA       0x71

volatile int century_register;

int is_update_in_progress(void);
unsigned char get_RTC_register(int reg);

void read_datetime(datetime* ptr)
{
    datetime dt, lastdt;
    unsigned char century = 0, last_century;
    unsigned char regB;

    /* Note: This uses the "read registers until you get the same values twice in a row" technique
             to avoid getting dodgy/inconsistent values due to RTC updates
     */

    while (is_update_in_progress());
    dt.secs = get_RTC_register(0x00);
    dt.mins = get_RTC_register(0x02);
    dt.hours = get_RTC_register(0x04);
    dt.day = get_RTC_register(0x07);
    dt.month = get_RTC_register(0x08);
    dt.year = get_RTC_register(0x09);
    if (century_register != 0)
        century = get_RTC_register(century_register);

    do
    {
        lastdt = dt;
        last_century = century;

        while (is_update_in_progress());
        dt.secs = get_RTC_register(0x00);
        dt.mins = get_RTC_register(0x02);
        dt.hours = get_RTC_register(0x04);
        dt.day = get_RTC_register(0x07);
        dt.month = get_RTC_register(0x08);
        dt.year = get_RTC_register(0x09);
        if (century_register != 0)
            century = get_RTC_register(century_register);
    }
    while (lastdt.secs != dt.secs || lastdt.mins != dt.mins || lastdt.hours != dt.hours ||
           lastdt.day != dt.day || lastdt.month != dt.month || lastdt.year != dt.year ||
           last_century != century);

    regB = get_RTC_register(0x0B);

    /* Convert BCD to binary values if necessary */
    if (!(regB & 0x04))
    {
        dt.secs = (dt.secs & 0x0F) + ((dt.secs/16)*10);
        dt.mins = (dt.mins & 0x0F) + ((dt.mins/16)*10);
        dt.hours = ( (dt.hours & 0x0F) + (((dt.hours & 0x70)/16)*10) ) | (dt.hours & 0x80);
        dt.day = (dt.day & 0x0F) + ((dt.day/16)*10);
        dt.month = (dt.month & 0x0F) + ((dt.month/16)*10);
        dt.year = (dt.year & 0x0F) + ((dt.year/16)*10);

        if (century_register != 0)
            century = (century & 0x0F) + ((century/16)*10);
    }

    /* Convert 12 hour clock to 24 hour if necessary */
    if (!(regB & 0x02) && (dt.hours & 0x80))
        dt.hours = ((dt.hours & 0x7F) + 12) % 24;

    /* Calculate the full (4-digit) year */
    if (century_register != 0)
        dt.year += century*100;
    else
    {
        dt.year += (CURRENT_YEAR/100)*100;
        if (dt.year < CURRENT_YEAR)
            dt.year += 100;
    }

    if (ptr)
        *ptr = dt;
}

void set_century_reg(int reg)
{
    century_register = reg;
}


int is_update_in_progress(void)
{
    outb(CMOS_ADDRESS, 0x0A);
    return (inb(CMOS_DATA) & 0x80);
}

unsigned char get_RTC_register(int reg)
{
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}


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

#ifndef _DATE_TIME_H_
#define _DATE_TIME_H_

typedef struct
{
    unsigned short secs;
    unsigned short mins;
    unsigned short hours;

    unsigned short day;
    unsigned short month;
    unsigned short year;
}datetime;

void read_datetime(datetime* ptr);

/* Sets the register from where the "century" value will be pulled.
   Can be retrieved from "Fixed ACPI Description Table", offset 108 */
void set_century_reg(int reg);

#endif /* _DATE_TIME_H_ */


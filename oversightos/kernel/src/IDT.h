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

#ifndef __IDT_H_
#define __IDT_H_

void IDT_Init(void);


typedef enum { GT_INT, GT_TASK, GT_TRAP }GateTypes;

typedef void (*funcPtr)(void);

void IDT_SetGate(int gate, funcPtr ptr, GateTypes type, int user, int storage);

void IRQ_Enable(int line);
void IRQ_Disable(int line);

#endif /* __IDT_H_ */


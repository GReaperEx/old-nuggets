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

#ifndef __OS_STRING_H_
#define __OS_STRING_H_

#include "OStypes.h"

#define NULL ((void*)0)

size_t OSstrlen(const char* str);

char* OSstrcpy(char* dst, const char* src);
char* OSstrncpy(char* dst, const char* src, size_t n);

char* OSstrcat(char* dst, const char* src);
char* OSstrncat(char* dst, const char* src, size_t n);

int OSstrcmp(const char* s1, const char* s2);
int OSstrncmp(const char* s1, const char* s2, size_t n);

#endif /* __OS_STRING_H_ */


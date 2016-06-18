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

#ifndef _OS_ALLOC_H_
#define _OS_ALLOC_H_

#include "paging.h"

/* In order to use any of these, "paging_initAlloc()" must have been called */

/* Allocation functions return NULL if the system fails to
   allocate the requested pages. */

/* Returns a pointer to the start of the allocated 4-KByte pages
   Pages don't have to be physically continuous */
void* OSalloc(size_t pageAmount, BOOL kernelSpace);

/* Resizes the allocated space. Acts as OSalloc if oldPtr==NULL
   "oldptr" should always be discarded, even when the size is reduced!
   ( unless NULL is returned, of course ) */
void* OSrealloc(void* oldptr, size_t oldAmount, size_t newAmount, BOOL kernelSpace);

/* Frees up to "amount" pages pointed to by "ptr" */
void OSfree(void* ptr, size_t amount);

/* Frees all allocated memory ( process-specific, kernel space stays intact ) */
void OSfreeAll(void);

#endif /* _OS_ALLOC_H_ */


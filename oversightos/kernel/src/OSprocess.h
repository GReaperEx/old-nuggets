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

#ifndef __OS_PROCESS_H_
#define __OS_PROCESS_H_

#include "OStypes.h"

#define MAX_PROCESSES (1 << 8)

/* The method for process switching is preemptive multitasking, using the PIT */

/* Gives 32-KBytes of stack space for the system( every syscall reserves 1-KByte ) */
/* At least one process should be created before calling this. If more than a single
   process are created, execution will start from the first one, ID #1 */
void Init_processes(void);

/* Interrupt Service Routine #32 */
void ISR32(void);


/* The external ID of a process. A value of zero means "absence of" */
typedef uint32_t OSprocess;

typedef struct
{
    /* Both must be page-aligned */
    void* srcAddress; /* This can be NULL, dstAddress will be filled with zeros */
    void* dstAddress;

    uint32_t sizeInPages;
    BOOL isReadWrite;
}SectionMap;

typedef enum {
    PS_INVALID,  /* The process doesn't exist */
    PS_NEWBORN,  /* It has just been created, haven't started yet */
    PS_STANDBY,  /* Locked and loaded, ready to be started */
    PS_PAUSED,   /* Explicitly asked to be passed via "OSprocessPause()" */
    PS_WAITING,  /* Waiting for an "external" source to bump it */
    PS_RUNNING,  /* Can be run at any time */
    PS_SLEEPING, /* Being idle for a set amount of time */
    PS_RETURNED, /* Has finished its execution normally */
    PS_HANGING   /* There was a problem, and the execution was aborted */
}OSprocStates;

typedef enum {
    PP_MINIMAL,  /* Gets the least CPU time, and lowest priority on devices */
    PP_LESSER,   /* Gets twice the CPU time than PP_MINIMAL, and somewhat higher device priority */
    PP_NORMAL,   /* Twice the CPU of PP_LESSER, and higher device priority */
    PP_ELEVATED, /* 50% more CPU than PP_NORMAL, but the same priority */
    PP_EXTREME   /* Twice the CPU of PP_NORMAL, and the highest device priority available */
}OSprocPriors;

/* Returns the ID of the newly created process */
OSprocess OSprocessCreate(OSprocess parent, const char* name, OSprocPriors priority);

/* Maps all the requested sections to their desired addresses */
/* Will fail if there are overlapping pages */
/* All source addresses don't exist after a successful call to this, as it transfers them to the new process */
BOOL OSprocessFeed(OSprocess pId, const SectionMap sectMap[], int sectAmount);

/* Set's up the process, preparing it for execution */
BOOL OSprocessStart(OSprocess pId, void* startAddress);

/* Stops the process dead in its tracks, if it was PS_RUNNING */
BOOL OSprocessPause(OSprocess pId);

/* Resumes execution after an OSprocessPause() command */
BOOL OSprocessResume(OSprocess pId);

/* Puts the process to sleep for that many milliseconds, only if it running though */
BOOL OSprocessSleep(OSprocess pId, size_t millis);

/* Assigns the value returned from the process, if its execution has ended */
BOOL OSprocessRetValue(OSprocess pId, int* retValuePtr);

/* Frees all allocated resources, passed process can't be used after this */
BOOL OSprocessDestroy(OSprocess pId);

BOOL OSprocessParent(OSprocess pId, OSprocess* parentId);

BOOL OSprocessName(OSprocess pId, char* nameBuffer, int maxLen);

OSprocess OSprocessID(void);

#endif /* __OS_PROCESS_H_ */


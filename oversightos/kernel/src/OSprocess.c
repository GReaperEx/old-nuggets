/* Copyright (C) 2014, GReaperEx(Marios F.)
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

#include "OSprocess.h"
#include "ports.h"
#include "IDT.h"
#include "OSalloc.h"
#include "OSstring.h"
#include "TextUI.h"

#define PIT_DATA_CHANNEL 0x40
#define PIT_MODE_CMD_REG 0x43

#define PROC_NAME_SIZE 64

typedef struct
{
    uint32_t EDI;
    uint32_t ESI;
    uint32_t EBP;
    uint32_t _useless; /* ESP */
    uint32_t EBX;
    uint32_t EDX;
    uint32_t ECX;
    uint32_t EAX;

    uint32_t codeAddress;
    uint32_t codeSegment;

    uint32_t flags;

    uint32_t stackAddress;
    uint32_t stackSegment;
}procIntInfo;

typedef struct
{
    uint8_t fpuState[512];

    procIntInfo registers;
    OSprocess pId;
}processInfo;

volatile struct processStateInfo
{
    char name[PROC_NAME_SIZE];

    OSprocess parent;
    OSprocStates state;
    OSprocPriors priority;

    void* PDTptr;

    /* Only valid if state==PS_SLEEPING */
    size_t millis;
    /* Only valid if state==PS_RETURNED */
    int retValue;
}procStateInfo[MAX_PROCESSES];

void* dummyPDT;

#define MAX_PROCESSES_MASK ((OSprocess)(MAX_PROCESSES - 1))

void Init_processes(void)
{
    /* Setting up a "dummy" process for when no process is runnable */
    dummyPDT = (void*)(getPT_entry((void*)0xFFFFFFFC)->lvalue & PDT_ADDRESS_MASK);
    /* Using the original paging table, since it'll go to waste anyway */
    uint8_t* dummyPage = paging_alloc((void*)0x100000, FALSE, NULL, FALSE, TRUE, TRUE);
    processInfo* procInfo = (processInfo*)dummyPage;
    uint32_t codeStart = 0x100000 + sizeof(processInfo);

    __asm__ __volatile__("pushfl \n\t" /* Saving the flags */
                         "popl %%eax \n\t"
                         "movl %%eax, %0 \n\t": "=m"(procInfo->registers.flags):: "%eax");

    procInfo->registers._useless = 0x101000;
    procInfo->registers.stackSegment = 0x23;
    procInfo->registers.stackAddress = 0x101000;
    procInfo->registers.codeSegment = 0x1B;
    procInfo->registers.codeAddress = codeStart;

    /* Just a little manually coded segment to halt until interrupted */
    ((uint8_t*)codeStart)[0] = 0x31;
    ((uint8_t*)codeStart)[1] = 0xC0; /* XOR EAX, EAX */
    ((uint8_t*)codeStart)[2] = 0xCD;
    ((uint8_t*)codeStart)[3] = 0x80; /* INT 0x80 */
    ((uint8_t*)codeStart)[4] = 0xEB;
    ((uint8_t*)codeStart)[5] = 0xFA; /* JMP -4 */

    /* Setting the PIT Mode/Command register */
    /* Channel 0, lobyte/hibyte, rate generator, binary mode */
    outb(PIT_MODE_CMD_REG, 0x34);

    /* Setting reload value for the PIT, 1193 for ~1000Hz */
    outb(PIT_DATA_CHANNEL, 0xA9);
    outb(PIT_DATA_CHANNEL, 0x04);
}


/* This will be called by our assembly code */
void _ScheduleTasks(procIntInfo intInfo)
{
    static OSprocess curProcess = MAX_PROCESSES_MASK;
    static uint32_t msLeftToRun = 0;
    static BOOL prevWasDummy = FALSE;

    static const uint32_t ProcTimes[] = { 1, 2, 4, 6, 8 };

    OSprocess startProcess = curProcess;
    BOOL foundIt = FALSE;

    int i;

    if (procStateInfo[curProcess].state == PS_RUNNING && msLeftToRun)
        --msLeftToRun;
    else
    {
        do {
            /* A quick way to wrap around the value */
            curProcess = (curProcess + 1) & MAX_PROCESSES_MASK;

            if (procStateInfo[curProcess].state == PS_RUNNING)
            {
                foundIt = TRUE;
                break;
            }
        } while (curProcess != startProcess);

        if (foundIt)
        {
            volatile processInfo* procInfo = (processInfo*)0x100000;
            char buffer[20];

            OSuinttostr(buffer, intInfo.codeAddress, 16);
            TextUI_puts("\nEIP: ");
            TextUI_puts(buffer);
            OSuinttostr(buffer, intInfo.stackAddress, 16);
            TextUI_puts(" ESP: ");
            TextUI_puts(buffer);
            TextUI_putchar(' ');

            msLeftToRun = ProcTimes[procStateInfo[curProcess].priority] - 1;

            if (!prevWasDummy)
            {
                /* Saving old process state */
                procInfo->registers = intInfo;
                __asm__ __volatile__("fxsave %0 \n\t"::"m"(procInfo->fpuState));
            }

            /* Loading new PDT */
            setPDTable(procStateInfo[curProcess].PDTptr);

            /* Fetching new process state */
            intInfo = procInfo->registers;
            __asm__ __volatile__("fxrstor %0 \n\t"::"m"(procInfo->fpuState));

            prevWasDummy = FALSE;
        }
        else  if (!prevWasDummy)
        {
            volatile processInfo* procInfo = (processInfo*)0x100000;

            /* Saving old process state */
            procInfo->registers = intInfo;
            __asm__ __volatile__("fxsave %0 \n\t"::"m"(procInfo->fpuState));

            setPDTable(dummyPDT);
            prevWasDummy = TRUE;
            intInfo = procInfo->registers;
        }
    }

    for (i = 0; i < MAX_PROCESSES; i++){
        if (procStateInfo[i].state == PS_SLEEPING)
        {
            if (--procStateInfo[i].millis == 0)
                procStateInfo[i].state = PS_RUNNING;
        }
    }
}

OSprocess OSprocessCreate(OSprocess parent, const char* name, OSprocPriors priority)
{
    OSprocess i;

    for (i = 0; i < MAX_PROCESSES; i++){
        if (procStateInfo[i].state == PS_INVALID)
        {
            void *newPDT = OSalloc(1, FALSE);
            void *oldPDT = (void*)(getPT_entry((void*)0xFFFFFFFC)->lvalue & PDT_ADDRESS_MASK);
            void *procInfoSector;

            if (newPDT == NULL)
                return 0;

            paging_setupPDT(newPDT);
            procStateInfo[i].PDTptr = (void*)(getPT_entry(newPDT)->lvalue & PDT_ADDRESS_MASK);
            /* Freeing the virtual address, it served its purpose */
            paging_free(newPDT, FALSE);

            setPDTable(procStateInfo[i].PDTptr);
            /* Loading the new PDT and setting the extra process-info page */
            procInfoSector = paging_alloc((void*)0x100000, FALSE, NULL, FALSE, TRUE, TRUE);
            ((processInfo*)procInfoSector)->pId = i + 1;

            OSstrncpy(procStateInfo[i].name, name, PROC_NAME_SIZE);
            procStateInfo[i].parent = parent;
            procStateInfo[i].state = PS_NEWBORN;
            procStateInfo[i].priority = priority;

            setPDTable(oldPDT);

            return i + 1;
        }
    }

    return 0;
}

BOOL checkSectionMap(const SectionMap sectMap[], int sectAmount);
void transferSection(void* dstPDT, SectionMap section);

BOOL OSprocessFeed(OSprocess pId, const SectionMap sectMap[], int sectAmount)
{
    int i;

    if (pId == 0 || pId > MAX_PROCESSES)
        return FALSE;

    if (procStateInfo[pId - 1].state != PS_NEWBORN)
        return FALSE;

    if (!checkSectionMap(sectMap, sectAmount))
        return FALSE;

    for (i = 0; i < sectAmount; i++){
        transferSection(procStateInfo[pId-1].PDTptr, sectMap[i]);
    }

    procStateInfo[pId-1].state = PS_STANDBY;

    return TRUE;
}

BOOL checkSectionMap(const SectionMap sectMap[], int sectAmount)
{
    int i, j;

    for (i = 0; i < sectAmount; i++){
        if (sectMap[i].sizeInPages == 0 || !isLegitAddress(sectMap[i].dstAddress) ||
            (sectMap[i].srcAddress != NULL && !isLegitAddress(sectMap[i].srcAddress)))
            return FALSE;

        for (j = i + 1; j < sectAmount; j++){
            uint32_t srcs[2] = { (uint32_t)(sectMap[i].srcAddress), (uint32_t)(sectMap[j].srcAddress) };
            uint32_t dsts[2] = { (uint32_t)(sectMap[i].dstAddress), (uint32_t)(sectMap[j].dstAddress) };
            uint32_t sizs[2] = { sectMap[i].sizeInPages*4096, sectMap[j].sizeInPages*4096};

            if (srcs[0] && srcs[1])
            {
                if ((srcs[1] <= srcs[0] && srcs[0] < srcs[1] + sizs[1]) ||
                    (srcs[0] <= srcs[1] && srcs[1] < srcs[0] + sizs[0]))
                    return FALSE;
            }

            if ((dsts[1] <= dsts[0] && dsts[0] < dsts[1] + sizs[1]) ||
                (dsts[0] <= dsts[1] && dsts[1] < dsts[0] + sizs[0]))
                return FALSE;
        }
    }

    return TRUE;
}

void transferSection(void* dstPDT, SectionMap section)
{
    void* oldPDT = (void*)(getPT_entry((void*)0xFFFFFFFC)->lvalue & PDT_ADDRESS_MASK);
    uint32_t i;

    for (i = 0; i < section.sizeInPages; i++){
        void* dstVirtual = (char*)section.dstAddress + i*4096;
        void* srcPhysical;

        if (section.srcAddress == NULL)
            srcPhysical = NULL;
        else
            srcPhysical = (void*)(getPT_entry((char*)section.srcAddress + i*4096)->lvalue & PDT_ADDRESS_MASK);

        paging_free((char*)section.srcAddress + i*4096, FALSE);

        setPDTable(dstPDT);

        paging_alloc(dstVirtual, TRUE, srcPhysical, FALSE, FALSE, section.isReadWrite);
        if (srcPhysical == NULL)
        {
            int j;
            for (j = 0; j < 1024; j++){
                ((uint32_t*)dstVirtual)[j] = 0;
            }
        }

        setPDTable(oldPDT);
    }
}

BOOL OSprocessStart(OSprocess pId, void* startAddress)
{
    void* oldPDT = (void*)(getPT_entry((void*)0xFFFFFFFC)->lvalue & PDT_ADDRESS_MASK);
    processInfo *procInfo = (void*)0x100000;

    if (pId == 0 || pId > MAX_PROCESSES)
        return FALSE;

    if (procStateInfo[pId - 1].state != PS_STANDBY)
        return FALSE;

    if (!isLegitAddress(startAddress))
        return FALSE;

    setPDTable(procStateInfo[pId-1].PDTptr);

    procInfo->registers.codeAddress = (uint32_t)startAddress;
    procInfo->registers.codeSegment = 0x1B;
    procInfo->registers._useless = 0xC0000000;
    procInfo->registers.stackAddress = 0xC0000000;
    procInfo->registers.stackSegment = 0x23;

    __asm__ __volatile__("pushfl \n\t" /* Saving the flags */
                         "popl %%eax \n\t"
                         "movl %%eax, %0 \n\t": "=m"(procInfo->registers.flags):: "%eax");

    procInfo->registers.flags |= 0x1 << 9; /* Setting the interrupt flag */

    __asm__ __volatile__("fxsave %0 \n\t"::"m"(procInfo->fpuState));

    paging_alloc((void*)(0xC0000000 - 4096), TRUE, NULL, FALSE, FALSE, TRUE);

    setPDTable(oldPDT);
    procStateInfo[pId-1].state = PS_RUNNING;

    return TRUE;
}

BOOL OSprocessPause(OSprocess pId)
{
    if (pId == 0 || pId > MAX_PROCESSES)
        return FALSE;

    if (procStateInfo[pId - 1].state != PS_RUNNING)
        return FALSE;

    procStateInfo[pId-1].state = PS_PAUSED;

    return TRUE;
}

BOOL OSprocessResume(OSprocess pId)
{
    if (pId == 0 || pId > MAX_PROCESSES)
        return FALSE;

    if (procStateInfo[pId - 1].state != PS_PAUSED)
        return FALSE;

    procStateInfo[pId-1].state = PS_RUNNING;

    return TRUE;
}

BOOL OSprocessSleep(OSprocess pId, size_t millis)
{
    if (pId == 0 || pId > MAX_PROCESSES)
        return FALSE;

    if (procStateInfo[pId - 1].state != PS_RUNNING)
        return FALSE;

    if (millis == 0)
        millis = 1;

    procStateInfo[pId-1].state = PS_SLEEPING;
    procStateInfo[pId-1].millis = millis;

    return TRUE;
}

BOOL OSprocessRetValue(OSprocess pId, int* retValuePtr)
{
    if (pId == 0 || pId > MAX_PROCESSES)
        return FALSE;

    if (procStateInfo[pId - 1].state != PS_RETURNED)
        return FALSE;

    if (!isLegitAddress(retValuePtr))
        return FALSE;

    *retValuePtr = procStateInfo[pId-1].retValue;

    return TRUE;
}

BOOL OSprocessDestroy(OSprocess pId)
{
    if (pId == 0 || pId > MAX_PROCESSES)
        return FALSE;

    if (procStateInfo[pId - 1].state != PS_INVALID)
    {
        void* oldPDT = (void*)(getPT_entry((void*)0xFFFFFFFC)->lvalue & PDT_ADDRESS_MASK);

        setPDTable(procStateInfo[pId-1].PDTptr);

        /* TODO: Free all handles here too */

        OSfreeAll();

        setPDTable(oldPDT);
    }

    procStateInfo[pId-1].state = PS_INVALID;

    return TRUE;
}

BOOL OSprocessParent(OSprocess pId, OSprocess* parentId)
{
    if (pId == 0 || pId > MAX_PROCESSES)
        return FALSE;

    *parentId = procStateInfo[pId-1].parent;

    return TRUE;
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))

BOOL OSprocessName(OSprocess pId, char* nameBuffer, int maxLen)
{
    if (pId == 0 || pId > MAX_PROCESSES)
        return FALSE;

    if (!isLegitAddress(nameBuffer) || !isLegitAddress(nameBuffer + maxLen - 1))
        return FALSE;

    OSstrncpy(nameBuffer, procStateInfo[pId-1].name, MIN(PROC_NAME_SIZE, maxLen));

    return TRUE;
}

OSprocess OSprocessID(void)
{
    return ((processInfo*)0x100000)->pId;
}

/* Copyright (C) 2014-2015, GReaperEx(Marios F.)
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

#include "OStypes.h"
#include "BIOSinfo.h"
#include "datetime.h"
#include "OSalloc.h"
#include "OSpower.h"
#include "TextUI.h"
#include "OSprocess.h"

#define PROC_NAME_SIZE 64

#define SYSCALL_HALT        0x0
#define SYSCALL_BIOS_INFO   0x1
#define SYSCALL_PNP_INFO    0x2
#define SYSCALL_DATETIME    0x3
#define SYSCALL_PALLOC      0x4
#define SYSCALL_PREALLOC    0x5
#define SYSCALL_PFREE       0x6
#define SYSCALL_SHUTDOWN    0x7
#define SYSCALL_REBOOT      0x8

#define SYSCALL_SEL_FORE_COLOR 0x9
#define SYSCALL_SEL_BACK_COLOR 0xA
#define SYSCALL_GET_FORE_COLOR 0xB
#define SYSCALL_GET_BACK_COLOR 0xC
#define SYSCALL_CLRSCR         0xD
#define SYSCALL_GET_CURSOR_POS 0xE
#define SYSCALL_SET_CURSOR_POS 0xF
#define SYSCALL_PUTCHAR        0x10
#define SYSCALL_PUTS           0x11
#define SYSCALL_GETCH          0x12
#define SYSCALL_GETCHE         0x13
#define SYSCALL_GETCHAR        0x14
#define SYSCALL_UNGETC         0x15
#define SYSCALL_GETS           0x16

#define SYSCALL_IS_KEY_DOWN 0x17
#define SYSCALL_IS_KEY_UP   0x18
#define SYSCALL_IS_LED_ON   0x19
#define SYSCALL_IS_LED_OFF  0x1A
#define SYSCALL_GET_KEY     0x1B

#define SYSCALL_PROC_CREATE    0x1C
#define SYSCALL_PROC_FEED      0x1D
#define SYSCALL_PROC_START     0x1E
#define SYSCALL_PROC_PAUSE     0x1F
#define SYSCALL_PROC_RESUME    0x20
#define SYSCALL_PROC_SLEEP     0x21
#define SYSCALL_PROC_RET_VALUE 0x22
#define SYSCALL_PROC_DESTROY   0x23
#define SYSCALL_PROC_PARENT    0x24

typedef struct
{
    uint32_t EDI;
    uint32_t ESI;
    uint32_t EBP;
    uint32_t _unused;
    uint32_t EBX;
    uint32_t EDX;
    uint32_t ECX;
    uint32_t EAX;
}GenRegs;

typedef struct
{
    uint16_t portIO_COM[4];
    uint16_t portIO_LPT[3];
    hdBitMap hardware_bmp;
    uint16_t TextMode_ColCount;
    uint16_t portIO_Video;
    uint8_t startupDisplayMode;
}BIOSinfo;

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

void syscall_halt(void);
BOOL syscall_BIOSinfo(BIOSinfo* ptr);
BOOL syscall_PnPinfo(PNP_Header* ptr);
BOOL syscall_datetime(datetime* ptr);
void* syscall_pAlloc(size_t pageAmount);
void* syscall_pRealloc(void* oldptr, size_t oldAmount, size_t newAmount);
void syscall_pFree(void* ptr, size_t amount);
void syscall_shutdown(void);
void syscall_reboot(void);

void syscall_selForeColor(int color);
void syscall_selBackColor(int color);
int syscall_getForeColor(void);
int syscall_getBackColor(void);
void syscall_clrscr(void);
void syscall_getCursorPos(int* row, int* col);
void syscall_setCursorPos(int row, int col);
void syscall_putchar(char c);
void syscall_puts(const char* str);
int syscall_getch(void);
int syscall_getche(void);
int syscall_getchar(void);
void syscall_ungetc(char c);
void syscall_gets(char* str, int maxLen);

int syscall_isKeyDown(Kbrd_Keys key);
int syscall_isKeyUp(Kbrd_Keys key);
int syscall_isLEDon(Kbrd_LEDs led);
int syscall_isLEDoff(Kbrd_LEDs led);
unsigned short syscall_getKey(void);

/* Processes are automatically the parents of those they create, and they can only manipulate their own children */
OSprocess syscall_procCreate(const char* name, OSprocPriors priority);
BOOL syscall_procFeed(OSprocess pId, const SectionMap sectMap[], int sectAmount);
BOOL syscall_procStart(OSprocess pId, void* startAddress);
BOOL syscall_procPause(OSprocess pId);
BOOL syscall_procResume(OSprocess pId);
BOOL syscall_procSleep(OSprocess pId, size_t millis);
BOOL syscall_procRetValue(OSprocess pId, int* retValuePtr);
BOOL syscall_procDestroy(OSprocess pId);
BOOL syscall_procParent(OSprocess* parentId); /* A process can only find its own parent */

void _handleRequest(GenRegs requestInfo)
{
    char buffer[50];

    switch (requestInfo.EAX)
    {
    case SYSCALL_HALT:
        syscall_halt();
    break;
    case SYSCALL_BIOS_INFO:
        requestInfo.EAX = syscall_BIOSinfo((void*)requestInfo.EDI);
    break;
    case SYSCALL_PNP_INFO:
        requestInfo.EAX = syscall_PnPinfo((void*)requestInfo.EDI);
    break;
    case SYSCALL_DATETIME:
        requestInfo.EAX = syscall_datetime((void*)requestInfo.EDI);
    break;
    case SYSCALL_PALLOC:
        requestInfo.EAX = (uint32_t)syscall_pAlloc(requestInfo.ECX);
    break;
    case SYSCALL_PREALLOC:
        requestInfo.EAX = (uint32_t)syscall_pRealloc((void*)requestInfo.ESI, requestInfo.ECX, requestInfo.EDX);
    break;
    case SYSCALL_PFREE:
        syscall_pFree((void*)requestInfo.ESI, requestInfo.ECX);
    break;
    case SYSCALL_SHUTDOWN:
        syscall_shutdown();
    break;
    case SYSCALL_REBOOT:
        syscall_reboot();
    break;
    case SYSCALL_SEL_FORE_COLOR:
        syscall_selForeColor(requestInfo.ECX);
    break;
    case SYSCALL_SEL_BACK_COLOR:
        syscall_selBackColor(requestInfo.ECX);
    break;
    case SYSCALL_GET_FORE_COLOR:
        requestInfo.EAX = syscall_getForeColor();
    break;
    case SYSCALL_GET_BACK_COLOR:
        requestInfo.EAX = syscall_getBackColor();
    break;
    case SYSCALL_CLRSCR:
        syscall_clrscr();
    break;
    case SYSCALL_GET_CURSOR_POS:
        syscall_getCursorPos((void*)requestInfo.ECX, (void*)requestInfo.EDX);
    break;
    case SYSCALL_SET_CURSOR_POS:
        syscall_setCursorPos(requestInfo.ECX, requestInfo.EDX);
    break;
    case SYSCALL_PUTCHAR:
        syscall_putchar((char)requestInfo.ECX);
    break;
    case SYSCALL_PUTS:
        syscall_puts((void*)requestInfo.ESI);
    break;
    case SYSCALL_GETCH:
        requestInfo.EAX = syscall_getch();
    break;
    case SYSCALL_GETCHE:
        requestInfo.EAX = syscall_getche();
    break;
    case SYSCALL_GETCHAR:
        requestInfo.EAX = syscall_getchar();
    break;
    case SYSCALL_UNGETC:
        syscall_ungetc((char)requestInfo.ECX);
    break;
    case SYSCALL_GETS:
        syscall_gets((void*)requestInfo.EDI, requestInfo.ECX);
    break;
    case SYSCALL_IS_KEY_DOWN:
        requestInfo.EAX = syscall_isKeyDown(requestInfo.ECX);
    break;
    case SYSCALL_IS_KEY_UP:
        requestInfo.EAX = syscall_isKeyUp(requestInfo.ECX);
    break;
    case SYSCALL_IS_LED_ON:
        requestInfo.EAX = syscall_isLEDon(requestInfo.ECX);
    break;
    case SYSCALL_IS_LED_OFF:
        requestInfo.EAX = syscall_isLEDoff(requestInfo.ECX);
    break;
    case SYSCALL_GET_KEY:
        requestInfo.EAX = syscall_getKey();
    break;
    case SYSCALL_PROC_CREATE:
        requestInfo.EAX = syscall_procCreate((void*)requestInfo.ESI, requestInfo.ECX);
    break;
    case SYSCALL_PROC_FEED:
        requestInfo.EAX = syscall_procFeed(requestInfo.ECX, (void*)requestInfo.ESI, requestInfo.EDX);
    break;
    case SYSCALL_PROC_START:
        requestInfo.EAX = syscall_procStart(requestInfo.ECX, (void*)requestInfo.ESI);
    break;
    case SYSCALL_PROC_PAUSE:
        requestInfo.EAX = syscall_procPause(requestInfo.ECX);
    break;
    case SYSCALL_PROC_RESUME:
        requestInfo.EAX = syscall_procResume(requestInfo.ECX);
    break;
    case SYSCALL_PROC_SLEEP:
        requestInfo.EAX = syscall_procSleep(requestInfo.ECX, requestInfo.EDX);
    break;
    case SYSCALL_PROC_RET_VALUE:
        requestInfo.EAX = syscall_procRetValue(requestInfo.ECX, (void*)requestInfo.EDI);
    break;
    case SYSCALL_PROC_DESTROY:
        requestInfo.EAX = syscall_procDestroy(requestInfo.ECX);
    break;
    case SYSCALL_PROC_PARENT:
        requestInfo.EAX = syscall_procParent((void*)requestInfo.EDI);
    break;
    }
}

void syscall_halt(void)
{
    OSprocessSleep(OSprocessID(), 3);
}

BOOL syscall_BIOSinfo(BIOSinfo* ptr)
{
    if (!ptr)
        return FALSE;

    ptr->portIO_COM[0] = portIO_COM1;
    ptr->portIO_COM[1] = portIO_COM2;
    ptr->portIO_COM[2] = portIO_COM3;
    ptr->portIO_COM[3] = portIO_COM4;

    ptr->portIO_LPT[0] = portIO_LPT1;
    ptr->portIO_LPT[1] = portIO_LPT2;
    ptr->portIO_LPT[2] = portIO_LPT3;

    ptr->hardware_bmp = hardware_bmp;
    ptr->startupDisplayMode = startupDisplayMode;
    ptr->TextMode_ColCount = TextMode_ColCount;
    ptr->portIO_Video = portIO_Video;

    return TRUE;
}

BOOL syscall_PnPinfo(PNP_Header* ptr)
{
    if (!ptr || PNPinfo.Signature != PNP_SIG)
        return FALSE;

    *ptr = PNPinfo;

    return TRUE;
}

BOOL syscall_datetime(datetime* ptr)
{
    if (!ptr)
        return FALSE;

    read_datetime(ptr);

    return TRUE;
}

void* syscall_pAlloc(size_t pageAmount)
{
    return OSalloc(pageAmount, FALSE);
}

void* syscall_pRealloc(void* oldptr, size_t oldAmount, size_t newAmount)
{
    return OSrealloc(oldptr, oldAmount, newAmount, FALSE);
}

void syscall_pFree(void* ptr, size_t amount)
{
    return OSfree(ptr, amount);
}

void syscall_shutdown(void)
{
    OSpower_shutdown();
}

void syscall_reboot(void)
{
    OSpower_reboot();
}

void syscall_selForeColor(int color)
{
    TextUI_SelForeColor(color);
}

void syscall_selBackColor(int color)
{
    TextUI_SelBackColor(color);
}

int syscall_getForeColor(void)
{
    return TextUI_GetForeColor();
}

int syscall_getBackColor(void)
{
    return TextUI_GetBackColor();
}

void syscall_clrscr(void)
{
    TextUI_ClrSrc();
}

void syscall_getCursorPos(int* row, int* col)
{
    if (isLegitAddress(row) && isLegitAddress(col))
        TextUI_GetCursorPos(row, col);
}

void syscall_setCursorPos(int row, int col)
{
    TextUI_SetCursorPos(row, col);
}

void syscall_putchar(char c)
{
    TextUI_putchar(c);
}

void syscall_puts(const char* str)
{
    if (isLegitAddress(str))
        TextUI_puts(str);
}

int syscall_getch(void)
{
    return TextUI_getch();
}

int syscall_getche(void)
{
    return TextUI_getche();
}

int syscall_getchar(void)
{
    return TextUI_getchar();
}

void syscall_ungetc(char c)
{
    TextUI_ungetc(c);
}

void syscall_gets(char* str, int maxLen)
{
    if (isLegitAddress(str) && isLegitAddress(str + maxLen))
        TextUI_gets(str, maxLen);
}

int syscall_isKeyDown(Kbrd_Keys key)
{
    return Kbrd_IsKeyDown(key);
}

int syscall_isKeyUp(Kbrd_Keys key)
{
    return Kbrd_IsKeyUp(key);
}

int syscall_isLEDon(Kbrd_LEDs led)
{
    return Kbrd_IsLEDon(led);
}

int syscall_isLEDoff(Kbrd_LEDs led)
{
    return Kbrd_IsLEDoff(led);
}

unsigned short syscall_getKey(void)
{
    return Kbrd_getKey();
}

OSprocess syscall_procCreate(const char* name, OSprocPriors priority)
{
    OSprocess parent = ((processInfo*)0x100000)->pId;

    if (isLegitAddress(name) && isLegitAddress(name + PROC_NAME_SIZE))
        return OSprocessCreate(parent, name, priority);

    return 0;
}

BOOL syscall_procFeed(OSprocess pId, const SectionMap sectMap[], int sectAmount)
{
    OSprocess parent;

    if (!isLegitAddress(sectMap) || !isLegitAddress(sectMap + sectAmount))
        return FALSE;

    if (!OSprocessParent(pId, &parent))
        return FALSE;

    if (parent != ((processInfo*)0x100000)->pId)
        return FALSE;

    return OSprocessFeed(pId, sectMap, sectAmount);
}

BOOL syscall_procStart(OSprocess pId, void* startAddress)
{
    OSprocess parent;

    if (!isLegitAddress(startAddress))
        return FALSE;

    if (!OSprocessParent(pId, &parent))
        return FALSE;

    if (parent != ((processInfo*)0x100000)->pId)
        return FALSE;

    return OSprocessStart(pId, startAddress);
}

BOOL syscall_procPause(OSprocess pId)
{
    OSprocess parent;

    if (!OSprocessParent(pId, &parent))
        return FALSE;

    if (parent != ((processInfo*)0x100000)->pId)
        return FALSE;

    return OSprocessPause(pId);
}

BOOL syscall_procResume(OSprocess pId)
{
    OSprocess parent;

    if (!OSprocessParent(pId, &parent))
        return FALSE;

    if (parent != ((processInfo*)0x100000)->pId)
        return FALSE;

    return OSprocessResume(pId);
}

BOOL syscall_procSleep(OSprocess pId, size_t millis)
{
    OSprocess parent;

    if (!OSprocessParent(pId, &parent))
        return FALSE;

    if (parent != ((processInfo*)0x100000)->pId)
        return FALSE;

    return OSprocessSleep(pId, millis);
}

BOOL syscall_procRetValue(OSprocess pId, int* retValuePtr)
{
    OSprocess parent;

    if (!isLegitAddress(retValuePtr))
        return FALSE;

    if (!OSprocessParent(pId, &parent))
        return FALSE;

    if (parent != ((processInfo*)0x100000)->pId)
        return FALSE;

    return OSprocessRetValue(pId, retValuePtr);
}

BOOL syscall_procDestroy(OSprocess pId)
{
    OSprocess parent;

    if (!OSprocessParent(pId, &parent))
        return FALSE;

    if (parent != ((processInfo*)0x100000)->pId)
        return FALSE;

    return OSprocessDestroy(pId);
}

BOOL syscall_procParent(OSprocess* parentId)
{
    if (!isLegitAddress(parentId))
        return FALSE;

    return OSprocessParent(((processInfo*)0x100000)->pId, parentId);
}



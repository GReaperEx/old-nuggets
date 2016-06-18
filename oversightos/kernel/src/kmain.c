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

#include "BIOSinfo.h"
#include "ports.h"
#include "mmapInfo.h"
#include "TextUI.h"
#include "IDT.h"
#include "exceptions.h"

#include "OSstring.h"
#include "datetime.h"
#include "OSalloc.h"

#include "SSE.h"
#include "OStypes.h"
#include "OSpower.h"

#include "paging.h"
#include "PCI.h"

#include "OSprocess.h"
#include "syscalls.h"

void gdt_install(void);
void init_paging(unsigned int kSize);

void _fixPagingHack(void);

unsigned int myTSS[26] = { 0x0 };

void _printHex(unsigned int value)
{
    static char hexArray[16] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
    };

    TextUI_putchar(hexArray[(value >> 28) & 0xF]);
    TextUI_putchar(hexArray[(value >> 24) & 0xF]);
    TextUI_putchar(hexArray[(value >> 20) & 0xF]);
    TextUI_putchar(hexArray[(value >> 16) & 0xF]);
    TextUI_putchar(hexArray[(value >> 12) & 0xF]);
    TextUI_putchar(hexArray[(value >>  8) & 0xF]);
    TextUI_putchar(hexArray[(value >>  4) & 0xF]);
    TextUI_putchar(hexArray[(value >>  0) & 0xF]);
}

unsigned int readUint(const char* buf);
int readInt(const char* buf);

void writeUint(unsigned int value);
void writeInt(int value);


extern int setupStart;
extern int setupEnd;
extern int kernelStart;
extern int kernelEnd;

void jumpToRing3(uint32_t state[5])
{
    int i;
    for (i = 0; i < 5; i++){
        state[i] = ((uint32_t*)(0x100000 + 512 + 8*4))[i];
    }

    __asm__ __volatile__("cli \n\t");

    IDT_SetGate(32, ISR32, GT_INT, 0, 0);
    IRQ_Enable(0);

    IDT_SetGate(128, ISR128, GT_INT, 1, 0);

    __asm__ __volatile__("mov $0x23, %%ax \n\t"
                         "mov %%ax, %%ds \n\t"
                         "mov %%ax, %%es \n\t"
                         "mov %%ax, %%fs \n\t"
                         "mov %%ax, %%gs \n\t"
                         "pushl %0 \n\t"
                         "pushl %1 \n\t"
                         "pushl %2 \n\t"
                         "pushl %3 \n\t"
                         "pushl %4 \n\t"
                         "iret \n\t":: "m"(state[4]), "m"(state[3]),
                                       "m"(state[2]), "m"(state[1]),
                                       "m"(state[0]));
}

/* This is the startup function of the OS */
void __attribute__((stdcall)) kmain(void* bootInfo)
{
    int i;
    char input[81];
#if 0
    int* allocated;
    unsigned int size;

    const char* abbrevs[ ] =
            { "Jan", "Fed", "Mar", "Apr", "May", "Jun",
              "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"   };
#endif
    pciDeviceInfoEx* devices;
    int devCount;
    uint32_t stack[5];

    __asm__ __volatile__("cli \n\t");

    /* Disabling the NMI */
    outb(0x70, inb(0x70) | 0x80);

    GatherMemMap(bootInfo);

    init_paging(((unsigned int)(&setupEnd) - (unsigned int)(&setupStart)) +
                ((unsigned int)(&kernelEnd) - (unsigned int)(&kernelStart)));
    gdt_install();
    _fixPagingHack();

    gatherBIOSinfo();

    TextUI_Init(0x7, 0x0);
    Kbrd_Init();
    IDT_Init();

    /* Disabling all IRQs */
    for (i = 0; i < 16; i++){
        IRQ_Disable(i);
    }
    IDT_SetGate(33, ISR33, GT_INT, 0, 0);
    IRQ_Enable(1); /* Except the keyboards' */

    setup_exceptions();

    /* Setting up the TSS */
    __asm__ __volatile__(
        "mov %%esp, %0 \n\t"
     :  "=m"(myTSS[1])
    );

    myTSS[2] = 0x10;
    myTSS[25] = 0 << 16;

    __asm__ __volatile__(
        "mov $0x28, %%ax \n\t"
        "ltr %%ax        \n\t"
    ::: "%ax"
    );

    /* Enabling the NMI */
    outb(0x70, inb(0x70) & 0x7F);

    __asm__ __volatile__("sti \n\t");

    TextUI_puts("Welcome to OverSight(OS)!\n");
    TextUI_puts("( Exaggerated comments erased :D )\n\n");

    if (PNPinfo.Signature != PNP_SIG)
        TextUI_puts("Failed to extract PnP information!\n");
    else
        TextUI_puts("PnP information extracted.\n");

    TextUI_puts("Gathering memory information, please wait...\n");
    paging_initAlloc();
    TextUI_puts("Information gathered, proceeding.\n");

    if (EnableSSE())
        TextUI_puts("SSE support enabled.\n");
    else
        TextUI_puts("Failed to enable SSE support! What sort of antique PC is this?!\n");

    TextUI_puts("Enumerating devices, please wait...\n");
    devices = pci_enumDevices(&devCount);
    TextUI_puts("Found a total of ");
    writeInt(devCount);
    TextUI_puts(" devices.\n");

    /* Freeing the memory used by pci_enumDevices() */
    OSfree(devices, (sizeof(pciDeviceInfoEx)*devCount + 4095)/4096);

    TextUI_puts("Initializing processes.\n");
    Init_processes();

    // Create a simple process for testing
    {
        OSprocess newProcess = OSprocessCreate(0, "MyDummyProcess", PP_MINIMAL);
        SectionMap sectMap;

        if (newProcess == 0)
            TextUI_puts("Couldn't create process\n");

        sectMap.srcAddress = OSalloc(1, FALSE);
        sectMap.dstAddress = (void*)0x200000;
        sectMap.sizeInPages = 1;
        sectMap.isReadWrite = FALSE;

        ((uint8_t*)sectMap.srcAddress)[0] = 0xB8;
        ((uint8_t*)sectMap.srcAddress)[1] = 0x10;
        ((uint8_t*)sectMap.srcAddress)[2] = 0x00;
        ((uint8_t*)sectMap.srcAddress)[3] = 0x00;
        ((uint8_t*)sectMap.srcAddress)[4] = 0x00;
        ((uint8_t*)sectMap.srcAddress)[5] = 0xB9;
        ((uint8_t*)sectMap.srcAddress)[6] = 0x64;
        ((uint8_t*)sectMap.srcAddress)[7] = 0x00;
        ((uint8_t*)sectMap.srcAddress)[8] = 0x00;
        ((uint8_t*)sectMap.srcAddress)[9] = 0x00;
        ((uint8_t*)sectMap.srcAddress)[10] = 0xCD;
        ((uint8_t*)sectMap.srcAddress)[11] = 0x80;
        ((uint8_t*)sectMap.srcAddress)[12] = 0x31;
        ((uint8_t*)sectMap.srcAddress)[13] = 0xC0;
        ((uint8_t*)sectMap.srcAddress)[14] = 0xCD;
        ((uint8_t*)sectMap.srcAddress)[15] = 0x80;
        ((uint8_t*)sectMap.srcAddress)[16] = 0xEB;
        ((uint8_t*)sectMap.srcAddress)[17] = 0xEE;
/*
        ((uint8_t*)sectMap.srcAddress)[0] = 0x31;
        ((uint8_t*)sectMap.srcAddress)[1] = 0xC0;
        ((uint8_t*)sectMap.srcAddress)[2] = 0xCD;
        ((uint8_t*)sectMap.srcAddress)[3] = 0x80;
        ((uint8_t*)sectMap.srcAddress)[4] = 0xEB;
        ((uint8_t*)sectMap.srcAddress)[5] = 0xFA;
*/

        if (!OSprocessFeed(newProcess, &sectMap, 1))
            TextUI_puts("Couldn't feed process\n");
        if (!OSprocessStart(newProcess, (void*)0x200000))
            TextUI_puts("Couldn't start process\n");
    }
/*
    // Create another simple process for testing
    {
        OSprocess newProcess = OSprocessCreate(0, "MyDummyProcess2", PP_MINIMAL);
        SectionMap sectMap;

        if (newProcess == 0)
            TextUI_puts("Couldn't create process\n");

        sectMap.srcAddress = OSalloc(1, FALSE);
        sectMap.dstAddress = (void*)0x200000;
        sectMap.sizeInPages = 1;
        sectMap.isReadWrite = FALSE;

        ((uint8_t*)sectMap.srcAddress)[0] = 0xB8;
        ((uint8_t*)sectMap.srcAddress)[1] = 0x10;
        ((uint8_t*)sectMap.srcAddress)[2] = 0x00;
        ((uint8_t*)sectMap.srcAddress)[3] = 0x00;
        ((uint8_t*)sectMap.srcAddress)[4] = 0x00;
        ((uint8_t*)sectMap.srcAddress)[5] = 0xB9;
        ((uint8_t*)sectMap.srcAddress)[6] = 0x64;
        ((uint8_t*)sectMap.srcAddress)[7] = 0x00;
        ((uint8_t*)sectMap.srcAddress)[8] = 0x00;
        ((uint8_t*)sectMap.srcAddress)[9] = 0x00;
        ((uint8_t*)sectMap.srcAddress)[10] = 0xCD;
        ((uint8_t*)sectMap.srcAddress)[11] = 0x80;
        ((uint8_t*)sectMap.srcAddress)[12] = 0x31;
        ((uint8_t*)sectMap.srcAddress)[13] = 0xC0;
        ((uint8_t*)sectMap.srcAddress)[14] = 0xCD;
        ((uint8_t*)sectMap.srcAddress)[15] = 0x80;
        ((uint8_t*)sectMap.srcAddress)[16] = 0xEB;
        ((uint8_t*)sectMap.srcAddress)[17] = 0xEE;

        if (!OSprocessFeed(newProcess, &sectMap, 1))
            TextUI_puts("Couldn't feed process\n");
        if (!OSprocessStart(newProcess, (void*)0x200000))
            TextUI_puts("Couldn't start process\n");
    }*/

    // Create another simple process for testing
    {
        OSprocess newProcess = OSprocessCreate(0, "MyDummyProcess3", PP_MINIMAL);
        SectionMap sectMap;

        if (newProcess == 0)
            TextUI_puts("Couldn't create process\n");

        sectMap.srcAddress = OSalloc(1, FALSE);
        sectMap.dstAddress = (void*)0x200000;
        sectMap.sizeInPages = 1;
        sectMap.isReadWrite = FALSE;

        ((uint8_t*)sectMap.srcAddress)[0] = 0xB8;
        ((uint8_t*)sectMap.srcAddress)[1] = 0x10;
        ((uint8_t*)sectMap.srcAddress)[2] = 0x00;
        ((uint8_t*)sectMap.srcAddress)[3] = 0x00;
        ((uint8_t*)sectMap.srcAddress)[4] = 0x00;
        ((uint8_t*)sectMap.srcAddress)[5] = 0xB9;
        ((uint8_t*)sectMap.srcAddress)[6] = 0x65;
        ((uint8_t*)sectMap.srcAddress)[7] = 0x00;
        ((uint8_t*)sectMap.srcAddress)[8] = 0x00;
        ((uint8_t*)sectMap.srcAddress)[9] = 0x00;
        ((uint8_t*)sectMap.srcAddress)[10] = 0xCD;
        ((uint8_t*)sectMap.srcAddress)[11] = 0x80;
        ((uint8_t*)sectMap.srcAddress)[12] = 0x31;
        ((uint8_t*)sectMap.srcAddress)[13] = 0xC0;
        ((uint8_t*)sectMap.srcAddress)[14] = 0xCD;
        ((uint8_t*)sectMap.srcAddress)[15] = 0x80;
        ((uint8_t*)sectMap.srcAddress)[16] = 0xEB;
        ((uint8_t*)sectMap.srcAddress)[17] = 0xEE;
/*
        ((uint8_t*)sectMap.srcAddress)[0] = 0x31;
        ((uint8_t*)sectMap.srcAddress)[1] = 0xC0;
        ((uint8_t*)sectMap.srcAddress)[2] = 0xCD;
        ((uint8_t*)sectMap.srcAddress)[3] = 0x80;
        ((uint8_t*)sectMap.srcAddress)[4] = 0xEB;
        ((uint8_t*)sectMap.srcAddress)[5] = 0xFA;
*/
        if (!OSprocessFeed(newProcess, &sectMap, 1))
            TextUI_puts("Couldn't feed process\n");
        if (!OSprocessStart(newProcess, (void*)0x200000))
            TextUI_puts("Couldn't start process\n");
    }

    jumpToRing3(stack);

    TextUI_puts("This should never be printed on the screen.");

    for (;;){
        __asm__ __volatile__ ("hlt \n\t");
    }

#if 0
    allocated = NULL;
    size = 0;
    /* This is just for show/testing */
    TextUI_putchar('\n');
    for (;;){
        TextUI_puts("o_O? ");
        TextUI_gets(input, 81);

        if (OSstrncmp(input, "help", 4) == 0)
        {
            if (input[4] == ' ')
            {
                if (OSstrcmp(&input[5], "help") == 0)
                    TextUI_puts("Syntax:\n\thelp [command]\nUsage:\n\tDisplays information about a command\n"
                                                                   "\tDisplays all commands' syntax if empty\n");
                else if (OSstrcmp(&input[5], "osinfo") == 0)
                    TextUI_puts("Syntax:\n\tosinfo\nUsage:\n\tDisplays information about the system\n");
                else if (OSstrcmp(&input[5], "color") == 0)
                    TextUI_puts("Syntax:\n\tcolor [XY]\nUsage:\n\tChanges foreground(X) and background(Y) colors\n"
                                                               "\tDisplays currents foregnd/backgnd colors if empty\n");
                else if (OSstrcmp(&input[5], "clrscr") == 0)
                    TextUI_puts("Syntax:\n\tclrscr\nUsage:\n\tClears the screen\n");
                else if (OSstrcmp(&input[5], "date") == 0)
                    TextUI_puts("Syntax:\n\tdate\nUsage:\n\tPrints the current date\n");
                else if (OSstrcmp(&input[5], "time") == 0)
                    TextUI_puts("Syntax:\n\ttime\nUsage:\n\tPrints the current time (HH:MM:SS)\n");
                else if (OSstrcmp(&input[5], "datetime") == 0)
                    TextUI_puts("Syntax:\n\tdatetime\nUsage:\n\tPrints both date and time\n");
                else if (OSstrcmp(&input[5], "peek") == 0)
                    TextUI_puts("Syntax:\n\tpeek <index>\nUsage:\n\tPrints the value pointed by \"index\"( decimal )\n"
                                                                 "\tPointed memory must be allocated\n");
                else if (OSstrcmp(&input[5], "poke") == 0)
                    TextUI_puts("Syntax:\n\tpeek <index> <newValue>\nUsage:\n\tSets \"index\" to \"newValue\" ( both decimal )\n"
                                                                            "\tPointed memory must me allocated\n");
                else if (OSstrcmp(&input[5], "alloc") == 0)
                    TextUI_puts("Syntax:\n\talloc <dwords>\nUsage:\n\tAllocates a new \"practice\" memory\n"
                                                                  "\tPrints the new buffer's location, if successful\n"
                                                                  "\t\"dwords\" is the amount of integers you want to have\n"
                                                                  "\tCan be accessed with peek/poke using indexing\n");
                else if (OSstrcmp(&input[5], "free") == 0)
                    TextUI_puts("Syntax:\n\tfree\nUsage:\n\tFrees the memory caught with \"alloc\"\n");
                else if (OSstrcmp(&input[5], "reboot") == 0)
                    TextUI_puts("Syntax:\n\treboot\nUsage:\n\tSafely stops all processes and reboots the system( if possible )\n");
                else if (OSstrcmp(&input[5], "shutdown") == 0)
                    TextUI_puts("Syntax:\n\tshutdown\nUsage:\n\tSafely stops all processes and shuts the system down( if possible )\n");
                else
                    TextUI_puts("Error: help : unknown command name\n");
            }
            else
                TextUI_puts("Commands:\n\tclrscr\n\tcolor [XY]\n\tdate\n\ttime\n\tdatetime\n\tosinfo\n\thelp [command]\n\t"
                                         "peek <index>\n\tpoke <index> <newValue>\n\talloc <dwords>\n\tfree\n\treboot\n\tshutdown\n");
        }
        else if (OSstrcmp(input, "osinfo") == 0)
        {
            TextUI_puts("\tOverSight(OS) v0.1.6\n"
                        "\tCopyright(C) 2012-2013 , GReaperEx(Marios F.)\n"
                        "\tLicense: GNU General Public License v3 or later\n");

            TextUI_puts("\tUsable RAM: ");

            OSftostr(input, OS_totalPages()/256.0f+0.005f, 10, 2, 0);
            TextUI_puts(input);
            TextUI_puts(" MBytes\n");

            TextUI_puts("\tPercentage of used RAM: ");

            OSftostr(input, (OS_usedPages()/(float)OS_totalPages())*100+0.005f, 10, 2, 0);
            TextUI_puts(input);
            TextUI_puts("%\n");

        }
        else if (OSstrncmp(input, "color", 5) == 0)
        {
            if (input[5] == ' ')
            {
                if (input[6] >= '0' && input[6] <= '9')
                    TextUI_SelForeColor(input[6] - '0');
                else if (input[6] >= 'A' && input[6] <= 'F')
                    TextUI_SelForeColor(input[6] - 'A' + 10);
                else if (input[6] >= 'a' && input[6] <= 'f')
                    TextUI_SelForeColor(input[6] - 'a' + 10);
                else
                    TextUI_puts("Error: color : malformed foreground color\n");

                if (input[7] >= '0' && input[7] <= '9')
                    TextUI_SelBackColor(input[7] - '0');
                else if (input[7] >= 'A' && input[7] <= 'F')
                    TextUI_SelBackColor(input[7] - 'A' + 10);
                else if (input[7] >= 'a' && input[7] <= 'f')
                    TextUI_SelForeColor(input[7] - 'a' + 10);
                else
                    TextUI_puts("Error: color : malformed background color\n");
            }
            else
            {
                static char hexes[ ] =
                { '0', '1', '2', '3', '4',
                  '5', '6', '7', '8', '9',
                  'A', 'B', 'C', 'D', 'E',
                  'F'                      };

                TextUI_puts("\tforegnd ");
                TextUI_putchar(hexes[TextUI_GetForeColor()]);
                TextUI_puts("; backgnd ");
                TextUI_putchar(hexes[TextUI_GetBackColor()]);
                TextUI_putchar('\n');
            }
        }
        else if (OSstrcmp(input, "clrscr") == 0)
            TextUI_ClrSrc();
        else if (OSstrcmp(input, "date") == 0)
        {
            datetime dt;

            read_datetime(&dt);

            TextUI_putchar('\t');

            /* Printing month */
            TextUI_puts(abbrevs[dt.month-1]);
            TextUI_putchar(' ');
            /* Printing day */
            TextUI_putchar('0' + dt.day / 10);
            TextUI_putchar('0' + dt.day % 10);

            if (dt.day != 11 && dt.day != 12)
            {
                switch (dt.day % 10)
                {
                    case 1:
                        TextUI_puts("st");
                    break;
                    case 2:
                        TextUI_puts("nd");
                    break;
                    case 3:
                        TextUI_puts("rd");
                    break;
                    default:
                        TextUI_puts("th");
                }
            }
            else
                TextUI_puts("th");

            TextUI_puts(", ");
            /* Printing year */
            TextUI_putchar('0' + dt.year/1000);
            TextUI_putchar('0' + (dt.year % 1000)/100);
            TextUI_putchar('0' + (dt.year % 100)/10);
            TextUI_putchar('0' + (dt.year % 10));

            TextUI_putchar('\n');

        }
        else if (OSstrcmp(input, "time") == 0)
        {
            datetime dt;

            read_datetime(&dt);

            TextUI_putchar('\t');

            /* Printing hours */
            TextUI_putchar('0' + dt.hours / 10);
            TextUI_putchar('0' + dt.hours % 10);
            TextUI_putchar(':');
            /* Printing minutes */
            TextUI_putchar('0' + dt.mins / 10);
            TextUI_putchar('0' + dt.mins % 10);
            TextUI_putchar(':');
            /* Printing seconds */
            TextUI_putchar('0' + dt.secs / 10);
            TextUI_putchar('0' + dt.secs % 10);

            TextUI_putchar('\n');
        }
        else if (OSstrcmp(input, "datetime") == 0)
        {
            datetime dt;

            read_datetime(&dt);

            TextUI_putchar('\t');

            /* Printing month */
            TextUI_puts(abbrevs[dt.month-1]);
            TextUI_putchar(' ');
            /* Printing day */
            TextUI_putchar('0' + dt.day / 10);
            TextUI_putchar('0' + dt.day % 10);

            if (dt.day != 11 && dt.day != 12)
            {
                switch (dt.day % 10)
                {
                    case 1:
                        TextUI_puts("st");
                    break;
                    case 2:
                        TextUI_puts("nd");
                    break;
                    case 3:
                        TextUI_puts("rd");
                    break;
                    default:
                        TextUI_puts("th");
                }
            }
            else
                TextUI_puts("th");

            TextUI_puts(", ");
            /* Printing year */
            TextUI_putchar('0' + dt.year/1000);
            TextUI_putchar('0' + (dt.year % 1000)/100);
            TextUI_putchar('0' + (dt.year % 100)/10);
            TextUI_putchar('0' + (dt.year % 10));

            TextUI_puts(" ; ");
            /* Printing hours */
            TextUI_putchar('0' + dt.hours / 10);
            TextUI_putchar('0' + dt.hours % 10);
            TextUI_putchar(':');
            /* Printing minutes */
            TextUI_putchar('0' + dt.mins / 10);
            TextUI_putchar('0' + dt.mins % 10);
            TextUI_putchar(':');
            /* Printing seconds */
            TextUI_putchar('0' + dt.secs / 10);
            TextUI_putchar('0' + dt.secs % 10);

            TextUI_putchar('\n');
        }
        else if (OSstrncmp(input, "peek ", 5) == 0)
        {
            if (input[5] >= '0' && input[5] <= '9')
            {
                unsigned int index = readUint(&input[5]);

                if (index >= size)
                    TextUI_puts("Error: peek : Tried to peek out of boundaries\n");
                else
                {
                    TextUI_puts("\tMemory(");
                    writeUint(index);
                    TextUI_puts(") = ");
                    writeInt(allocated[index]);
                    TextUI_putchar('\n');
                }
            }
            else
                TextUI_puts("Error: peek : Expected index but got garbage\n");
        }
        else if (OSstrncmp(input, "poke ", 5) == 0)
        {
            if (input[5] >= '0' && input[5] <= '9')
            {
                unsigned int index = readUint(&input[5]);

                for (i = 5; i < 80; i++){
                    if ((input[i] < '0' || input[i] > '9') && input[i] != '+' && input[i] != '-')
                        break;
                }
                if (input[i] == ' ') ++i;

                if (input[i] >= '0' && input[i] <= '9')
                {
                    int value = readInt(&input[i]);

                    if (index >= size)
                        TextUI_puts("Error: poke : Tried to poke out of boundaries\n");
                    else
                    {
                        allocated[index] = value;

                        TextUI_puts("\tMemory(");
                        writeUint(index);
                        TextUI_puts(") set to ");
                        writeInt(value);
                        TextUI_putchar('\n');
                    }
                }
                else
                    TextUI_puts("Error: poke : Expected newValue but got garbage\n");
            }
            else
                TextUI_puts("Error: poke : Expected index but got garbage\n");
        }
        else if (OSstrncmp(input, "alloc ", 6) == 0)
        {
            unsigned int dwords;
            int *oldPtr;

            if ((input[6] >= '0' && input[6] <= '9') || input[6] == '+')
            {
                dwords = readUint(&input[6]);

                oldPtr = allocated;
                allocated = OSrealloc(allocated, (size*4 + 4095)/4096, (dwords*4 + 4095)/4096, FALSE);
                if (allocated == NULL)
                {
                    TextUI_puts("Error: alloc : Failed to allocate requested amount\n");
                    allocated = oldPtr;
                }
                else
                {
                    TextUI_puts("\tSuccessfully allocated requested amount\n");
                    size = dwords;
                }
            }
            else
                TextUI_puts("Error: alloc : Expected dwords but got garbage\n");
        }
        else if (OSstrcmp(input, "free") == 0)
        {
            OSfree(allocated, (size*4 + 4095)/4096);
            size = 0;
            TextUI_puts("\tAllocated memory released\n");
        }
        else if (OSstrcmp(input, "reboot") == 0)
            OSpower_reboot();
        else if (OSstrcmp(input, "shutdown") == 0)
            OSpower_shutdown();
        else if (input[0] != '\0')
            TextUI_puts("Does not compute... :p\n");
    }
#endif
}


unsigned int readUint(const char* buf)
{
    unsigned int result;

    OSstrtouint(&result, buf, 10);

    return result;
}

int readInt(const char* buf)
{
    int result;

    OSstrtoint(&result, buf, 10);

    return result;
}

void writeUint(unsigned int value)
{
    char buf[11];

    OSuinttostr(buf, value, 10);

    TextUI_puts(buf);
}

void writeInt(int value)
{
    char buf[12];

    OSinttostr(buf, value, 10);

    TextUI_puts(buf);
}


unsigned long kernel_pDir[1024] __attribute__((aligned(4096)));
unsigned long low_pTable[1024] __attribute__((aligned(4096)));
unsigned long kernel_pTables[255][1024] __attribute__((aligned(4096)));

typedef struct __attribute__((packed))
{
    unsigned short limit;
    unsigned int base;
}gdt_ptr;

gdt_ptr gp;

void _fixPagingHack(void)
{
    int i;
    unsigned long kernel_pdPtr = (unsigned long)kernel_pDir + 0x40000000;

    for (i = 1; i < 255; i++)
    {
        kernel_pDir[i] = 0;
    }

    for (i = 256; i < 1024; i++)
    {
        low_pTable[i] = 0;
    }

    /* Reloading the paging table */
    setPDTable((void*)kernel_pdPtr);
}
void init_paging(unsigned int kSize)
{
    unsigned long kernel_pdPtr = 0;
    unsigned long low_ptPtr = 0;
    unsigned long kernel_ptPtr = 0;
    int i, j;
    unsigned int k4KB;

    /* Converting linear to physical addresses */
    kernel_pdPtr = (unsigned long)kernel_pDir + 0x40000000;
    low_ptPtr = (unsigned long)low_pTable + 0x40000000;
    kernel_ptPtr = (unsigned long)kernel_pTables + 0x40000000;

    /* Identity paging the 1st 4-MBytes */
    for (i = 0; i < 1024; i++)
    {
        low_pTable[i] = (i*4096) | 0x3;
    }
    kernel_pDir[0] = low_ptPtr | 0x7;

    for (i = 1; i < 255; i++)
    {
        kernel_pDir[i] = (kernel_ptPtr + (i - 1)*4096) | 0x3;
    }

    /* Setting the kernel directory entries */
    for (; i < 1023; i++)
    {
        kernel_pDir[i] = (kernel_ptPtr + (i - 768)*4096) | 0x3;
    }

    /* Last pdt entry will point to itself */
    kernel_pDir[1023] = kernel_pdPtr | 0x3;
    k4KB = (kSize + 4095)/4096 + 1;

    /* Setting the kernel page entries */
    for (i = 0; k4KB != 0; i++)
    {
        for (j = 0; k4KB != 0 && j < 1024; j++, k4KB--)
        {
            if (i == 0 && j < 256)
                ++k4KB;
            else
                kernel_pTables[i][j] = (i*4194304 + j*4096) | 0x3;
        }
    }

    /* Activating paging */
    paging_enable((void*)kernel_pdPtr);
}

typedef struct __attribute__((packed))
{
    unsigned short limit_low16;
    unsigned short base_low16;
    unsigned char base_mid8;
    unsigned char access;
    unsigned char limit_high4_flags;
    unsigned char base_high8;
}gdt_entry;

gdt_entry gdt[10] =
{
    /* Null descriptor */
    { 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00 },
    /* Supervisor code descriptor */
    { 0xFFFF, 0x0000, 0x00, 0x9A, 0xCF, 0x00 },
    /* Supervisor data descriptor */
    { 0xFFFF, 0x0000, 0x00, 0x92, 0xCF, 0x00 },
    /* Userspace code descriptor */
    { 0xFFFF, 0x0000, 0x00, 0xFA, 0xCF, 0x00 },
    /* Userspace data descriptor */
    { 0xFFFF, 0x0000, 0x00, 0xF2, 0xCF, 0x00 },

};

void gdt_flush(void);

void gdt_install(void)
{
    /* Inserting the TSS inside the gdt */
    gdt[5].limit_low16 = 104;
    gdt[5].base_low16 = (unsigned short)(unsigned int)myTSS;
    gdt[5].base_mid8 = (unsigned char)((unsigned int)myTSS >> 16);
    gdt[5].access = 0x89;
    gdt[5].limit_high4_flags = 0x40;
    gdt[5].base_high8 = (unsigned char)((unsigned int)myTSS >> 24);

    gp.limit = sizeof(gdt_entry)*10 - 1;
    gp.base = (unsigned int)(&gdt);

    gdt_flush();
}



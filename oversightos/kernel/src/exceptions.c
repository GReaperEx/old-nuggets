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
#include "IDT.h"
#include "OStypes.h"
#include "ports.h"
#include "OSprocess.h"

/* Forward declarations of CPU exceptions */
void ISR0(void);
void ISR1(void);
void ISR2(void);
void ISR3(void);
void ISR4(void);
void ISR5(void);
void ISR6(void);
void ISR7(void);
void ISR8(void);
void ISR10(void);
void ISR11(void);
void ISR12(void);
void ISR13(void);
void ISR14(void);
void ISR16(void);
void ISR17(void);
void ISR18(void);
void ISR19(void);

typedef struct __attribute__((packed)) __attribute__((aligned(4)))
{
    unsigned int Present    : 1;
    unsigned int Write      : 1;
    unsigned int User       : 1;
    unsigned int ResrvWrite : 1;
    unsigned int InstrFetch : 1;
}ErrorCode;

typedef struct __attribute__((packed)) __attribute__((aligned(4)))
{
    unsigned int External  : 1;
    unsigned int TableType : 2;
    unsigned int Index     : 13;
}SelErrorCode;

typedef struct
{
    uint32_t EDI;
    uint32_t ESI;
    uint32_t EBP;
    uint32_t ESP;
    uint32_t EBX;
    uint32_t EDX;
    uint32_t ECX;
    uint32_t EAX;
}GenRegs;

/* Faults */

void div_by_zero(GenRegs regs, const void* opcode);
void bound_exceeded(GenRegs regs, const void* opcode);
void invalid_opcode(GenRegs regs, const void* opcode);
void dev_not_avail(GenRegs regs, const void* opcode);
void invalid_TSS(GenRegs regs, SelErrorCode errCode, const void* opcode);
void seg_not_present(GenRegs regs, SelErrorCode errCode, const void* opcode);
void stack_seg_fault(GenRegs regs, SelErrorCode errCode, const void* opcode);
void general_protection(GenRegs regs, SelErrorCode errCode, const void* opcode);
void page_fault(GenRegs regs, ErrorCode errCode, const void* opcode);
void x87_exception(GenRegs regs, const void* opcode_after);
void align_check(GenRegs regs, const void* opcode);
void SIMD_exception(GenRegs regs, const void* opcode);
void non_mask_int(GenRegs regs, const void* opcode);


/* Traps */

void debug(GenRegs regs, const void* opcode_var); /* Fault/Trap */
void breakpoint(GenRegs regs, const void* opcode_after);
void overflow(GenRegs regs, const void* opcode_after);

/* Aborts */

void double_fault(GenRegs regs);
void mach_check(GenRegs regs, const void* opcode_var);


void setup_exceptions(void)
{
    IDT_SetGate(0, ISR0, GT_INT, 0, 0);
    IDT_SetGate(1, ISR1, GT_TRAP, 0, 0);
    IDT_SetGate(2, ISR2, GT_INT, 0, 0);
    IDT_SetGate(3, ISR3, GT_TRAP, 3, 0);
    IDT_SetGate(4, ISR4, GT_TRAP, 0, 0);
    IDT_SetGate(5, ISR5, GT_INT, 0, 0);
    IDT_SetGate(6, ISR6, GT_INT, 0, 0);
    IDT_SetGate(7, ISR7, GT_INT, 0, 0);
    IDT_SetGate(8, ISR8, GT_INT, 0, 0);
    IDT_SetGate(10, ISR10, GT_INT, 0, 0);
    IDT_SetGate(11, ISR11, GT_INT, 0, 0);
    IDT_SetGate(12, ISR12, GT_INT, 0, 0);
    IDT_SetGate(13, ISR13, GT_INT, 0, 0);
    IDT_SetGate(14, ISR14, GT_INT, 0, 0);
    IDT_SetGate(16, ISR16, GT_INT, 0, 0);
    IDT_SetGate(17, ISR17, GT_INT, 0, 0);
    IDT_SetGate(18, ISR18, GT_INT, 0, 0);
    IDT_SetGate(19, ISR19, GT_INT, 0, 0);
};

/* Currently, all exceptions just halt forever */
static void _hang(void)
{
    TextUI_puts("System is halting: Please restart your PC ...\n");

    /* Disabling the NMI */
    outb(0x70, inb(0x70) | 0x80);

    __asm__ __volatile__(
       "cli \n\t"
       "hlt \n\t"
    );
}

void _hexdump(GenRegs* regs)
{
    char buf[11];

    TextUI_puts("General registers:\n");

    TextUI_puts("EAX = 0x");
    OSuinttostr(buf, regs->EAX, 16);
    TextUI_puts(buf);

    TextUI_puts("; ECX = 0x");
    OSuinttostr(buf, regs->ECX, 16);
    TextUI_puts(buf);

    TextUI_puts("; EDX = 0x");
    OSuinttostr(buf, regs->EDX, 16);
    TextUI_puts(buf);

    TextUI_puts("; EBX = 0x");
    OSuinttostr(buf, regs->EBX, 16);
    TextUI_puts(buf);

    TextUI_putchar('\n');

    TextUI_puts("ESP = 0x");
    OSuinttostr(buf, regs->ESP, 16);
    TextUI_puts(buf);

    TextUI_puts("; EBP = 0x");
    OSuinttostr(buf, regs->EBP, 16);
    TextUI_puts(buf);

    TextUI_puts("; ESI = 0x");
    OSuinttostr(buf, regs->ESI, 16);
    TextUI_puts(buf);

    TextUI_puts("; EDI = 0x");
    OSuinttostr(buf, regs->EDI, 16);
    TextUI_puts(buf);

    TextUI_putchar('\n');
}

void div_by_zero(GenRegs regs, const void* opcode)
{
    char buf[11];

    TextUI_puts("System Exception: Divide by zero\n\tOpcode at address 0x");
    OSuinttostr(buf, (unsigned int)opcode, 16);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    _hexdump(&regs);
    _hang();
}

void bound_exceeded(GenRegs regs, const void* opcode)
{
    char buf[11];

    TextUI_puts("System Exception: Bound range exceeded\n\tOpcode at address 0x");
    OSuinttostr(buf, (unsigned int)opcode, 16);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    _hexdump(&regs);
    _hang();
}

void invalid_opcode(GenRegs regs, const void* opcode)
{
    char buf[11];

    char nameBuffer[64];
    if (!OSprocessName(OSprocessID(), nameBuffer, 64))
        OSstrcpy(nameBuffer, "UNKNOWN");

    TextUI_puts("\nCaused by process: ");
    TextUI_puts(nameBuffer);
    TextUI_putchar('\n');

    TextUI_puts("System Exception: Invalid opcode\n\tOpcode at address 0x");
    OSuinttostr(buf, (unsigned int)opcode, 16);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    _hexdump(&regs);
    _hang();
}

void dev_not_avail(GenRegs regs, const void* opcode)
{
    char buf[11];

    TextUI_puts("System Exception: Device not available\n\tOpcode at address 0x");
    OSuinttostr(buf, (unsigned int)opcode, 16);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    _hexdump(&regs);
    _hang();
}

void invalid_TSS(GenRegs regs, SelErrorCode errCode, const void* opcode)
{
    char buf[11];
    static char* Names[ ] = { "GDT", "IDT", "LDT", "IDT" };

    TextUI_puts("System Exception: Invalid TSS\n");

    TextUI_puts("\tWas external: ");
    TextUI_puts(errCode.External ? "True" : "False");
    TextUI_putchar('\n');

    TextUI_puts("\tDescriptor index: ");
    OSinttostr(buf, errCode.Index, 10);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    TextUI_puts("\tDescriptor is in: ");
    TextUI_puts(Names[errCode.TableType]);
    TextUI_putchar('\n');

    TextUI_puts("\tOpcode at address 0x");
    OSuinttostr(buf, (unsigned int)opcode, 16);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    _hexdump(&regs);
    _hang();
}

void seg_not_present(GenRegs regs, SelErrorCode errCode, const void* opcode)
{
    char buf[11];
    static char* Names[ ] = { "GDT", "IDT", "LDT", "IDT" };

    TextUI_puts("System Exception: Segment not present\n");

    TextUI_puts("\tWas external: ");
    TextUI_puts(errCode.External ? "True" : "False");
    TextUI_putchar('\n');

    TextUI_puts("\tDescriptor index: ");
    OSinttostr(buf, errCode.Index, 10);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    TextUI_puts("\tDescriptor is in: ");
    TextUI_puts(Names[errCode.TableType]);
    TextUI_putchar('\n');

    TextUI_puts("\tOpcode at address 0x");
    OSuinttostr(buf, (unsigned int)opcode, 16);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    _hexdump(&regs);
    _hang();
}

void stack_seg_fault(GenRegs regs, SelErrorCode errCode, const void* opcode)
{
    char buf[11];
    static char* Names[ ] = { "GDT", "IDT", "LDT", "IDT" };

    TextUI_puts("System Exception: Stack-Segment fault\n");

    TextUI_puts("\tWas external: ");
    TextUI_puts(errCode.External ? "True" : "False");
    TextUI_putchar('\n');

    TextUI_puts("\tDescriptor index: ");
    OSinttostr(buf, errCode.Index, 10);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    TextUI_puts("\tDescriptor is in: ");
    TextUI_puts(Names[errCode.TableType]);
    TextUI_putchar('\n');

    TextUI_puts("\tOpcode at address 0x");
    OSuinttostr(buf, (unsigned int)opcode, 16);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    _hexdump(&regs);
    _hang();
}

void general_protection(GenRegs regs, SelErrorCode errCode, const void* opcode)
{
    char buf[11];
    static char* Names[ ] = { "GDT", "IDT", "LDT", "IDT" };

    TextUI_puts("System Exception: General protection\n");

    TextUI_puts("\tWas external: ");
    TextUI_puts(errCode.External ? "True" : "False");
    TextUI_putchar('\n');

    TextUI_puts("\tDescriptor index: ");
    OSinttostr(buf, errCode.Index, 10);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    TextUI_puts("\tDescriptor is in: ");
    TextUI_puts(Names[errCode.TableType]);
    TextUI_putchar('\n');

    TextUI_puts("\tOpcode at address 0x");
    OSuinttostr(buf, (unsigned int)opcode, 16);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    _hexdump(&regs);
    _hang();
}

void page_fault(GenRegs regs, ErrorCode errCode, const void* opcode)
{
    char buf[11];
    unsigned int pageAddress;

    char nameBuffer[64];
    if (!OSprocessName(OSprocessID(), nameBuffer, 64))
        OSstrcpy(nameBuffer, "UNKNOWN");

    TextUI_puts("\nCaused by process: ");
    TextUI_puts(nameBuffer);
    TextUI_putchar('\n');

    __asm__ __volatile__("mov %%cr2, %0 \n\t": "=a"(pageAddress));

    TextUI_puts("System Exception: Page fault\n");

    TextUI_puts("\tWas present: ");
    TextUI_puts(errCode.Present ? "True" : "False");
    TextUI_putchar('\n');

    TextUI_puts("\tWas caused by a: ");
    TextUI_puts(errCode.Write ? "Write" : "Read");
    TextUI_putchar('\n');

    TextUI_puts("\tWas user mode: ");
    TextUI_puts(errCode.User ? "True" : "False");
    TextUI_putchar('\n');

    TextUI_puts("\tWas write to reserved: ");
    TextUI_puts(errCode.ResrvWrite ? "True" : "False");
    TextUI_putchar('\n');

    TextUI_puts("\tWas instruction fetch: ");
    TextUI_puts(errCode.InstrFetch ? "True" : "False");
    TextUI_putchar('\n');

    TextUI_puts("\tPage at address 0x");
    OSuinttostr(buf, pageAddress, 16);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    TextUI_puts("\tOpcode at address 0x");
    OSuinttostr(buf, (unsigned int)opcode, 16);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    _hexdump(&regs);
    _hang();
}

void x87_exception(GenRegs regs, const void* opcode_after)
{
    char buf[11];

    TextUI_puts("System Exception: x87 floating-point exception\n\tNext opcode at address 0x");
    OSuinttostr(buf, (unsigned int)opcode_after, 16);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    _hexdump(&regs);
    _hang();
}

void align_check(GenRegs regs, const void* opcode)
{
    char buf[11];

    TextUI_puts("System Exception: Alignment check\n\tOpcode at address 0x");
    OSuinttostr(buf, (unsigned int)opcode, 16);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    _hexdump(&regs);
    _hang();
}

void SIMD_exception(GenRegs regs, const void* opcode)
{
    char buf[11];

    TextUI_puts("System Exception: SIMD floating-point exception\n\tOpcode at address 0x");
    OSuinttostr(buf, (unsigned int)opcode, 16);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    _hexdump(&regs);
    _hang();
}

void non_mask_int(GenRegs regs, const void* opcode)
{
    char buf[11];

    TextUI_puts("System Exception: RAM and/or hardware error!\n\tOpcode at address 0x");
    OSuinttostr(buf, (unsigned int)opcode, 16);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    _hexdump(&regs);
    _hang();
}

void debug(GenRegs regs, const void* opcode_var)
{
    char buf[11];

    TextUI_puts("System Exception: Debug not implemented\n\tNext opcode at address 0x");
    OSuinttostr(buf, (unsigned int)opcode_var, 16);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    _hexdump(&regs);
    _hang();
}

void breakpoint(GenRegs regs, const void* opcode_after)
{
    char buf[11];

    TextUI_puts("System Exception: Breakpoint not implemented\n\tNext opcode at address 0x");
    OSuinttostr(buf, (unsigned int)opcode_after, 16);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    _hexdump(&regs);
    _hang();
}

void overflow(GenRegs regs, const void* opcode_after)
{
    char buf[11];

    TextUI_puts("System Exception: Overflow interrupt not implemented\n\tNext opcode at address 0x");
    OSuinttostr(buf, (unsigned int)opcode_after, 16);
    TextUI_puts(buf);
    TextUI_putchar('\n');

    _hexdump(&regs);
    _hang();
}

void double_fault(GenRegs regs)
{
    TextUI_puts("System Exception: Double fault!\n");

    _hexdump(&regs);
    _hang();
}

void mach_check(GenRegs regs, const void* opcode_var)
{
    TextUI_puts("System Exception: Machine check not implemented\n");

    _hexdump(&regs);
    _hang();
}



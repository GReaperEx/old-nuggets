; Copyright (C) 2012-2013, GReaperEx(Marios F.)
; This file is part of OverSight(OS)

; OverSight(OS) is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.

; OverSight(OS) is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.

; You should have received a copy of the GNU General Public License
; along with OverSight(OS).  If not, see <http://www.gnu.org/licenses/>.

;   ;   ;   ;   ;   ;   ;   ;   ;   ;   ;   ;   ;   ;   ;
;  x86 assembly, NASM, OverSight(OS) Kernel forwarder   ;
;   ;   ;   ;   ;   ;   ;   ;   ;   ;   ;   ;   ;   ;   ;

global _frontier
extern kmain

; Multiboot constants
%define MULTIBOOT_PAGE_ALIGN   1
%define MULTIBOOT_MEMORY_INFO  2
%define MULTIBOOT_HEADER_MAGIC 0x1BADB002
%define MULTIBOOT_HEADER_FLAGS (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO)
%define MULTIBOOT_CHECKSUM     (-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS))

section ._mbHeader

; Multiboot header (needed to boot from GRUB)
ALIGN 4
multiboot_header:
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_CHECKSUM
	resb 40

	JMP _frontier

section .text

extern myTSS

%macro PUSH_STACK 0
    SUB DWORD [myTSS + 4], 1024 ; "Pushing" 1024 bytes
%endmacro

%macro POP_STACK 0
    ADD DWORD [myTSS + 4], 1024 ; "Popping" 1024 bytes
%endmacro

_frontier:
    LGDT [trickgdt]
    MOV AX, 0x10
    MOV DS, AX
    MOV FS, AX
    MOV GS, AX
    MOV SS, AX
    ; Just a little hack
    MOV AX, 0x18
    MOV ES, AX

    JMP 0x08:higherhalf
higherhalf:
    MOV DWORD [myTSS + 4], sys_stack - 1024
    MOV ESP, sys_stack
    PUSH EBX
    CALL kmain

    JMP $

global gdt_flush
extern gp

gdt_flush:
    LGDT [gp]
    MOV AX, 0x10
    MOV DS, AX
    MOV ES, AX
    MOV FS, AX
    MOV GS, AX
    MOV SS, AX

    JMP 0x08:flush2
flush2:
    RET

;*** Exceptions Start Here ***;

global ISR0
extern div_by_zero

ISR0:
    PUSH_STACK
    PUSHA

    CALL div_by_zero

    MOV AL, 0x20
    OUT 0x20, AL

    POPA
    POP_STACK

    IRET

global ISR1
extern debug

ISR1:
    PUSH_STACK
    PUSHA

    CALL debug

    POPA
    POP_STACK

    IRET

global ISR2
extern non_mask_int

ISR2:
    PUSH_STACK
    PUSHA

    CALL non_mask_int

    POPA
    POP_STACK

    IRET

global ISR3
extern breakpoint

ISR3:
    PUSH_STACK
    PUSHA

    CALL breakpoint

    POPA
    POP_STACK

    IRET

global ISR4
extern overflow

ISR4:
    PUSH_STACK
    PUSHA

    CALL overflow

    POPA
    POP_STACK

    IRET

global ISR5
extern bound_exceeded

ISR5:
    PUSH_STACK
    PUSHA

    CALL bound_exceeded

    POPA
    POP_STACK

    IRET

global ISR6
extern invalid_opcode

ISR6:
    PUSH_STACK
    PUSHA

    CALL invalid_opcode

    POPA
    POP_STACK

    IRET

global ISR7
extern dev_not_avail

ISR7:
    PUSH_STACK
    PUSHA

    CALL dev_not_avail

    POPA
    POP_STACK

    IRET

global ISR8
extern double_fault

ISR8:
    PUSH_STACK
    PUSHA

    CALL double_fault

    POPA
    POP_STACK

    IRET

global ISR10
extern invalid_TSS

ISR10:
    PUSH_STACK
    PUSHA

    CALL invalid_TSS

    POPA
    POP_STACK

    IRET

global ISR11
extern seg_not_present

ISR11:
    PUSH_STACK
    PUSHA

    CALL seg_not_present

    POPA
    POP_STACK

    IRET

global ISR12
extern stack_seg_fault

ISR12:
    PUSH_STACK
    PUSHA

    CALL stack_seg_fault

    POPA
    POP_STACK

    IRET

global ISR13
extern general_protection

ISR13:
    PUSH_STACK
    PUSHA

    CALL general_protection

    POPA
    POP_STACK

    IRET

global ISR14
extern page_fault

ISR14:
    PUSH_STACK
    PUSHA

    CALL page_fault

    POPA
    POP_STACK

    IRET

global ISR16
extern x87_exception

ISR16:
    PUSH_STACK
    PUSHA

    CALL x87_exception

    POPA
    POP_STACK

    IRET

global ISR17
extern align_check

ISR17:
    PUSH_STACK
    PUSHA

    CALL align_check

    POPA
    POP_STACK

    IRET

global ISR18
extern mach_check

ISR18:
    PUSH_STACK
    PUSHA

    CALL mach_check

    POPA
    POP_STACK

    IRET

global ISR19
extern SIMD_exception

ISR19:
    PUSH_STACK
    PUSHA

    CALL SIMD_exception

    POPA
    POP_STACK

    IRET

;***  Exceptions End Here  ***;

global ISR32
extern _ScheduleTasks

ISR32:
    PUSH_STACK
    PUSHA

    CALL _ScheduleTasks

    MOV AL, 0x20
    OUT 0x20, AL

    POPA
    POP_STACK

    IRET


global ISR33
extern _TranslateScancodes

ISR33:
    PUSH_STACK
    PUSHA

    IN AL, 0x60
    AND EAX, 0xFF
    PUSH EAX
    CALL _TranslateScancodes

    MOV AL, 0x20
    OUT 0x20, AL

    POPA
    POP_STACK

    IRET

global ISR128
extern _handleRequest

ISR128:
    PUSH_STACK
    PUSHA

    CALL _handleRequest

    POPA
    POP_STACK

    IRET

section .setup

trickgdt:
	dw gdt_end - gdt - 1
	dd gdt

gdt:
	dd 0, 0
	db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0x40
	db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0x40
	db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0x00

gdt_end:

section .bss

resb 0x8000
sys_stack:




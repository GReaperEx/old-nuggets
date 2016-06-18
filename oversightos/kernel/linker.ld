ENTRY(_frontier)
OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
    . = 0x100000;
    ._mbHeader :
    {
        *(._mbHeader)
    }
    
    setupStart = .;

    .setup :
    {
        *(.setup)
        *(.eh_frame)
    }
    
    setupEnd = .;

    . += 0xC0000000;
    
    kernelStart = .;
    
    .text : AT(ADDR(.text) - 0xC0000000)
    {
        *(.text)
    }
    
    .data ALIGN (4096) : AT(ADDR(.data) - 0xC0000000)
    {
        *(.data)
        *(.rodata*)
    }
    
    .bss ALIGN (4096) : AT(ADDR(.bss) - 0xC0000000)
    {
        *(COMMON)
        *(.bss)
        *(.sbss)
    }
    
    kernelEnd = .;
}


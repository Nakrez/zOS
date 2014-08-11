OUTPUT_FORMAT("elf32-i386")
ENTRY(_start)

SECTIONS
{
    . = 0x400000;

    .text :
    {
        *(.text)
    }

    .rodata :
    {
        *(.rodata .rodata.*)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(.bss)
        *(.sbss)
    }
}


OUTPUT_FORMAT("elf32-i386")
ENTRY(kernel_main)

SECTIONS
{
    . = 0xC0020000;

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

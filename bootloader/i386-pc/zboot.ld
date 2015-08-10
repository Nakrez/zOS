OUTPUT_FORMAT("elf32-i386")
ENTRY(zboot_start)

SECTIONS
{
    . = 0x300000 + SIZEOF_HEADERS;

    .multiboot :
    {
        *(.multiboot)
    }

    .text :
    {
        *(.text)
        *(.rodata)
        *(.data)
        *(.bss)
    }

    _bend = ALIGN(4096);
}

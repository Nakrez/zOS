OUTPUT_FORMAT("elf32-i386")
ENTRY(zboot_start)

SECTIONS
{
    . = 0x10000;

    .multiboot :
    {
        *(.multiboot)
    }

    .text :
    {
        *(.text)
    }
}

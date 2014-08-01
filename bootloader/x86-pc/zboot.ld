OUTPUT_FORMAT("elf32-i386")
ENTRY(zboot_start)

SECTIONS
{
    . = 1M;

    .multiboot :
    {
        *(.multiboot)
    }

    .text :
    {
        *(.text)
    }
}

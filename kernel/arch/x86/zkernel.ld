OUTPUT_FORMAT("elf32-i386")
ENTRY(kernel_main)

SECTIONS
{
    . = 0xC0100000;

    .text :
    {
        *(.text)
    }
}

#include <kernel/console.h>

#include <arch/vga_text.h>
#include <arch/serial.h>

#include <glue/console.h>

struct console_glue console_glue_dispatcher =
{
    i386_pc_console_init,
    i386_pc_console_putc,
    i386_pc_console_color,
    i386_pc_console_clear,
};

int i386_pc_console_init(void)
{
    int ret;

    ret = x86_vga_text_init();
    ret &= x86_serial_init();

    return ret;
}

int i386_pc_console_putc(char c)
{
    int ret;

    ret = x86_vga_text_putc(c);
    ret &= x86_serial_putc(c);

    return ret;
}

int i386_pc_console_color(enum console_color c)
{
    int ret;

    ret = x86_vga_text_color(c);
    ret &= x86_serial_color(c);

    return ret;
}

int i386_pc_console_clear(void)
{
    int ret;

    ret = x86_vga_text_clear();
    ret &= x86_serial_clear();

    return ret;
}

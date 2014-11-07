#include <string.h>

#include <arch/console.h>
#include <arch/io.h>

# define CONSOLE_ADDR 0xC00B8000
# define YMAX 25
# define XMAX 80

# define X86_CONSOLE_BLUE 0x1
# define X86_CONSOLE_GREEN 0x2
# define X86_CONSOLE_RED 0x4
# define X86_CONSOLE_GREY 0x7

struct console
{
    int x;
    int y;
    char color;
};

static struct console x86_console;

static void x86_update_cursor(void)
{
    uint16_t cursor = x86_console.x + x86_console.y * XMAX;

    outb(0x3D4, 14);
    outb(0x3D5, (cursor >> 8) & 0xFF);
    outb(0x3D4, 15);
    outb(0x3D5, cursor & 0xFF);
}

static inline void x86_console_clear_line(int line)
{
    uint16_t *mem = (uint16_t *)CONSOLE_ADDR;

    for (int x = 0; x < XMAX; ++x)
        *(mem + x + line * XMAX) = (x86_console.color << 8) | ' ';
}

static void x86_console_scroll(void)
{
    uint16_t *mem = (uint16_t *)CONSOLE_ADDR;

    memcpy(mem, mem + XMAX, (YMAX - 1) * XMAX * 2);

    x86_console_clear_line(24);

    x86_update_cursor();
}

int x86_console_init(void)
{
    x86_console.x = 0;
    x86_console.y = 0;
    x86_console.color = X86_CONSOLE_GREY;

    x86_update_cursor();

    return 1;
}

int x86_console_putc(char c)
{
    if (x86_console.x == XMAX)
    {
        x86_console.x = 0;
        ++x86_console.y;
    }

    if (x86_console.y == YMAX)
    {
        x86_console.y = YMAX - 1;
        x86_console_scroll();
    }

    if (c == '\n')
    {
        ++x86_console.y;
        x86_console.x = 0;

        outb(0x3f8, '\n');

        if (x86_console.y == YMAX)
        {
            x86_console.y = YMAX - 1;
            x86_console_scroll();
        }
    }
    else
    {
        char *addr = (char *)CONSOLE_ADDR + x86_console.y * XMAX * 2 +
                     x86_console.x * 2;

        (*addr++) = c;
        (*addr) = x86_console.color;

        outb(0x3f8, c);

        ++x86_console.x;
    }

    x86_update_cursor();

    return 1;
}

int x86_console_color(enum console_color c)
{
    switch (c)
    {
        case COLOR_GREY:
            x86_console.color = X86_CONSOLE_GREY;
            break;
        case COLOR_RED:
            x86_console.color = X86_CONSOLE_RED;
            break;
        case COLOR_BLUE:
            x86_console.color = X86_CONSOLE_BLUE;
            break;
        case COLOR_GREEN:
            x86_console.color = X86_CONSOLE_GREEN;
            break;
    }

    return 1;
}

int x86_console_clear(void)
{
    x86_console.x = 0;
    x86_console.y = 0;

    for (int y = 0; y < YMAX; ++y)
        x86_console_clear_line(y);

    x86_update_cursor();

    return 1;
}

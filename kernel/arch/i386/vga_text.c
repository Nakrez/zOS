#include <string.h>

#include <arch/vga_text.h>
#include <arch/io.h>

# define CONSOLE_ADDR 0xC00B8000
# define YMAX 25
# define XMAX 80

# define X86_VGA_TEXT_BLUE 0x1
# define X86_VGA_TEXT_GREEN 0x2
# define X86_VGA_TEXT_RED 0x4
# define X86_VGA_TEXT_GREY 0x7

struct cursor
{
    int x;
    int y;
    char color;
};

static struct cursor vga_cursor;

static void update_cursor(void)
{
    uint16_t cursor = vga_cursor.x + vga_cursor.y * XMAX;

    outb(0x3D4, 14);
    outb(0x3D5, (cursor >> 8) & 0xFF);
    outb(0x3D4, 15);
    outb(0x3D5, cursor & 0xFF);
}

static inline void clear_line(int line)
{
    uint16_t *mem = (uint16_t *)CONSOLE_ADDR;

    for (int x = 0; x < XMAX; ++x)
        *(mem + x + line * XMAX) = (vga_cursor.color << 8) | ' ';
}

static void scroll(void)
{
    uint16_t *mem = (uint16_t *)CONSOLE_ADDR;

    memcpy(mem, mem + XMAX, (YMAX - 1) * XMAX * 2);

    clear_line(24);

    update_cursor();
}

int x86_vga_text_init(void)
{
    vga_cursor.x = 0;
    vga_cursor.y = 0;
    vga_cursor.color = X86_VGA_TEXT_GREY;

    update_cursor();

    return 1;
}

int x86_vga_text_putc(char c)
{
    if (vga_cursor.x == XMAX)
    {
        vga_cursor.x = 0;
        ++vga_cursor.y;
    }

    if (vga_cursor.y == YMAX)
    {
        vga_cursor.y = YMAX - 1;
        scroll();
    }

    if (c == '\n')
    {
        ++vga_cursor.y;
        vga_cursor.x = 0;

        if (vga_cursor.y == YMAX)
        {
            vga_cursor.y = YMAX - 1;
            scroll();
        }
    }
    else
    {
        char *addr = (char *)CONSOLE_ADDR + vga_cursor.y * XMAX * 2 +
                     vga_cursor.x * 2;

        (*addr++) = c;
        (*addr) = vga_cursor.color;

        ++vga_cursor.x;
    }

    update_cursor();

    return 1;
}

int x86_vga_text_color(enum console_color c)
{
    switch (c)
    {
        case COLOR_GREY:
            vga_cursor.color = X86_VGA_TEXT_GREY;
            break;
        case COLOR_RED:
            vga_cursor.color = X86_VGA_TEXT_RED;
            break;
        case COLOR_BLUE:
            vga_cursor.color = X86_VGA_TEXT_BLUE;
            break;
        case COLOR_GREEN:
            vga_cursor.color = X86_VGA_TEXT_GREEN;
            break;
    }

    return 1;
}

int x86_vga_text_clear(void)
{
    vga_cursor.x = 0;
    vga_cursor.y = 0;

    for (int y = 0; y < YMAX; ++y)
        clear_line(y);

    update_cursor();

    return 1;
}

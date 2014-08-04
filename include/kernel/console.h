#ifndef CONSOLE_H
# define CONSOLE_H

extern struct console_glue __console;

enum console_color
{
    COLOR_GREY,
    COLOR_RED,
    COLOR_BLUE,
    COLOR_GREEN,
};

enum console_type
{
    T_INF,
    T_ERR,
    T_OK,
};

struct console_glue
{
    void (*init)(void);
    void (*putc)(char c);
    void (*color)(enum console_color);
    void (*clear)(void);
};

void console_init(void);
void console_clear(void);
void console_puts(const char *s);
void console_printf(const char *s, ...);
void console_message(enum console_type t, const char *s, ...);

#endif /* !CONSOLE_H */

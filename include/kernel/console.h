#ifndef CONSOLE_H
# define CONSOLE_H

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
    int (*init)(void);
    int (*putc)(char c);
    int (*color)(enum console_color);
    int (*clear)(void);
};

extern struct console_glue console_glue_dispatcher;

void console_init(void);
void console_clear(void);
void console_puts(const char *s);
void console_printf(const char *s, ...);
void console_message(enum console_type t, const char *s, ...);

#endif /* !CONSOLE_H */

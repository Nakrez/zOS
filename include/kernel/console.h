#ifndef CONSOLE_H
# define CONSOLE_H

extern struct console_glue __console;

struct console_glue
{
    void (*init)(void);
    void (*putc)(char c);
    void (*color)(char c);
    void (*clear)(void);
};

void console_init(void);
void console_clear(void);
void console_puts(const char *s);

#endif /* !CONSOLE_H */

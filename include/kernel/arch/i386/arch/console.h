#ifndef ARCH_X86_CONSOLE_H
# define ARCH_X86_CONSOLE_H

# include <kernel/console.h>

int x86_console_init(void);
int x86_console_putc(char c);
int x86_console_color(enum console_color c);
int x86_console_clear(void);

#endif /* !ARCH_X86_CONSOLE_H */

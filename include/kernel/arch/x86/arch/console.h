#ifndef ARCH_X86_CONSOLE_H
# define ARCH_X86_CONSOLE_H

# include <kernel/console.h>

void x86_console_init(void);
void x86_console_putc(char c);
void x86_console_color(enum console_color c);
void x86_console_clear(void);

#endif /* !ARCH_X86_CONSOLE_H */

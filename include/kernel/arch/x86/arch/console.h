#ifndef ARCH_X86_CONSOLE_H
# define ARCH_X86_CONSOLE_H

void x86_console_init(void);
void x86_console_putc(char c);
void x86_console_color(char c);
void x86_console_clear(void);

#endif /* !ARCH_X86_CONSOLE_H */

#ifndef ARCH_X86_VGA_TEXT_H
# define ARCH_X86_VGA_TEXT_H

# include <kernel/console.h>

int x86_vga_text_init(void);
int x86_vga_text_putc(char c);
int x86_vga_text_color(enum console_color c);
int x86_vga_text_clear(void);

#endif /* !ARCH_X86_VGA_TEXT_H */

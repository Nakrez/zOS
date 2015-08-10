#ifndef GLUE_I386_PC_CONSOLE_H
# define GLUE_I386_PC_CONSOLE_H

int i386_pc_console_init(void);
int i386_pc_console_putc(char c);
int i386_pc_console_color(enum console_color c);
int i386_pc_console_clear(void);

#endif /* !GLUE_I386_PC_CONSOLE_H */

#ifndef I386_SERIAL_H
# define I386_SERIAL_H

# define COM1_DATA 0x3f8
# define COM1_IRQ_ENABLE (COM1_DATA + 1)
# define COM1_FIFO_CTRL (COM1_DATA + 2)
# define COM1_LINE_CTRL (COM1_DATA + 3)
# define COM1_MODEM_CTRL (COM1_DATA + 4)
# define COM1_LINE_STATUS (COM1_DATA + 5)

int x86_serial_init(void);
int x86_serial_putc(char c);
int x86_serial_color(enum console_color c);
int x86_serial_clear(void);

#endif /* !I386_SERIAL_H */

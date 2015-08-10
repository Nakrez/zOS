#include <kernel/console.h>

#include <arch/serial.h>
#include <arch/io.h>

int x86_serial_init(void)
{
    /* Disable interrupts */
    outb(COM1_IRQ_ENABLE, 0x0);

    /* Set baud rate */
    outb(COM1_LINE_CTRL, 0x80);
    outb(COM1_DATA, 0x03);
    outb(COM1_IRQ_ENABLE, 0x0);

    /* 8 bits, one stop, no parity, no break */
    outb(COM1_LINE_CTRL, 0x3);

    /* Enable FIFO, clear it, no dma, 14 bytes threshold */
    outb(COM1_FIFO_CTRL, 0xC7);

    outb(COM1_MODEM_CTRL, 0x0);

    return 1;
}

int x86_serial_putc(char c)
{
    /* Wait for com1 to be ready */
    while ((inb(COM1_LINE_STATUS) & 0x20) == 0)
        ;

    outb(COM1_DATA, c);

    return 1;
}

int x86_serial_color(enum console_color c)
{
    (void) c;

    return 1;
}

int x86_serial_clear(void)
{
    return 1;
}

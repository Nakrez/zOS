#include <arch/pic.h>
#include <arch/io.h>

void pic_initialize(void)
{
    outb(PIC_MASTER_CMD, 0x11);
    outb(PIC_SLAVE_CMD, 0x11);

    /* Master irq vector start at 0x20 (32) in IDT */
    outb(PIC_MASTER_DATA, 0x20);

    /* Slave irq vector start at 0x28 (40) in IDT */
    outb(PIC_SLAVE_DATA, 0x28);

    /* Tell master PIC that slave PIC is on irq 2 */
    outb(PIC_MASTER_DATA, 0x4);

    /* Tell slave PIC it own cascade (itself) */
    outb(PIC_SLAVE_DATA, 0x2);

    /* ICW4 */
    outb(PIC_MASTER_DATA, 0x1);
    outb(PIC_SLAVE_DATA, 0x1);

    /* Mask all interruption */
    outb(PIC_MASTER_DATA, 0xFB);
    outb(PIC_SLAVE_DATA, 0xFF);
}

void pic_acnowledge(int irq)
{
    if (irq > 7)
        outb(PIC_SLAVE_CMD, PIC_EOI);

    outb(PIC_MASTER_CMD, PIC_EOI);
}

void pic_mask(int irq)
{
    uint16_t port;
    uint8_t value;

    if (irq < 8)
        port = PIC_MASTER_DATA;
    else
    {
        port = PIC_SLAVE_DATA;
        irq -= 8;
    }

    value = inb(port);
    value |= (1 << irq);

    outb(port, value);
}

void pic_unmask(int irq)
{
    uint16_t port;
    uint8_t value;

    if (irq < 8)
        port = PIC_MASTER_DATA;
    else
    {
        port = PIC_SLAVE_DATA;
        irq -= 8;
    }

    value = inb(port);
    value &= ~(1 << irq);

    outb(port, value);
}

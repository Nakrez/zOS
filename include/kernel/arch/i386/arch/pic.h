#ifndef I386_PIC_H
# define I386_PIC_H

# define PIC_MASTER_CMD 0x20
# define PIC_MASTER_DATA 0x21

# define PIC_SLAVE_CMD 0xA0
# define PIC_SLAVE_DATA 0xA1

# define PIC_START_IRQ 0x20
# define PIC_END_IRQ 0x2F

# define PIC_EOI 0x20

# define PIC_IRQ_PIT 0x20

void pic_initialize(void);
void pic_acnowledge(int irq);
void pic_mask(int irq);
void pic_unmask(int irq);

#endif /* !I386_PIC_H */

#ifndef I386_IDT_H
# define I386_IDT_H

# include <kernel/types.h>

# define MAX_IRQ_NUMBER 0xFF

# define IRQ_PAGE_FAULT 14

# define IRQ_SYSCALL 0x80

# define INTERRUPT_GATE 0x8E00
# define TRAP_GATE 0xEF00

struct idt_ptr
{
    uint16_t size;
    uint32_t ptr;
} __attribute__ ((packed));

struct idt_entry
{
    uint16_t offsetl;
    uint16_t select;
    uint16_t type;
    uint16_t offseth;
} __attribute__ ((packed));

void idt_initialize(void);

#endif /* !I386_IDT_H */

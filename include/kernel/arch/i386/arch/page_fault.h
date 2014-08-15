#ifndef ARCH_I386_PAGE_FAULT_H
# define ARCH_I386_PAGE_FAULT_H

# include <arch/cpu.h>

# define PAGE_FAULT_NON_PRESENT (1 << 0)
# define PAGE_FAULT_WRITE (1 << 1)
# define PAGE_FAULT_USER (1 << 2)
# define PAGE_FAULT_RESERVED (1 << 3)
# define PAGE_FAULT_FETCH (1 << 4)

void page_fault_handler(struct irq_regs *regs);

#endif /* !ARCH_I386_PAGE_FAULT_H */

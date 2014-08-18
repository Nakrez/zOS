#ifndef ARCH_I386_THREAD_H
# define ARCH_I386_THREAD_H

# include <kernel/thread.h>

# define THREAD_MAX_STACK_SIZE (0x10 * PAGE_SIZE)
# define THREAD_STACK_BASE 0xBFFFE000

int i386_thread_create(struct process *p, struct thread *t, uintptr_t eip);
int i386_thread_duplicate(struct thread *thread, struct irq_regs *regs);
int i386_thread_current(void);

#endif /* !ARCH_I386_THREAD_H */

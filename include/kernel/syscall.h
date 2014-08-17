#ifndef SYSCALL_H
# define SYSCALL_H

# include <kernel/event.h>

# include <arch/cpu.h>

# define SYSCALL_MAX 0x4

struct syscall
{
    int num;

    /* Pointer on return value */
    int *ret;

    int arg1;
    int arg2;
    int arg3;
    int arg4;
    int arg5;

    struct irq_regs *regs;
};

typedef int (*syscall_callback)(struct syscall *interface);

struct syscall_glue
{
    int (*init)(void);
    int (*convert)(struct irq_regs *, struct syscall *);
};

extern struct syscall_glue syscall_glue_dispatcher;

void syscall_initialize(void);

void syscall_handler(struct irq_regs *regs);

#endif /* !SYSCALL_H */

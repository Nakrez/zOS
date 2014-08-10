#ifndef SYSCALL_H
# define SYSCALL_H

# include <kernel/event.h>

# define SYSCALL_MAX 0x1

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
};

typedef int (*syscall_callback)(struct syscall *interface);

struct glue_syscall
{
    void (*init)(void);
    void (*convert)(struct irq_regs *, struct syscall *);
};

extern struct glue_syscall _syscall;

void syscall_initialize(void);

void syscall_handler(struct irq_regs *regs);

#endif /* !SYSCALL_H */

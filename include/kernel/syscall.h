#ifndef SYSCALL_H
# define SYSCALL_H

# include <kernel/event.h>

# include <arch/cpu.h>

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

/* Syscalls */

/* Process */
int sys_usleep(struct syscall *interface);
int sys_exit(struct syscall *interface);
int sys_fork(struct syscall *interface);
int sys_getpid(struct syscall *interface);
int sys_gettid(struct syscall *interface);

/* Thread */
int sys_thread_create(struct syscall *interface);

#endif /* !SYSCALL_H */

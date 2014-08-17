#ifndef PROCESS_H
# define PROCESS_H

# include <kernel/types.h>
# include <kernel/as.h>

# include <arch/cpu.h>

# define PROCESS_MAX_PID 0xFF

# define PROCESS_TYPE_KERNEL 0
# define PROCESS_TYPE_USER (1 << 1)

/*
 * If set code is an area of memory that is an ELF binary, otherwise it is
 * interpreted as start address
 */
# define PROCESS_FLAG_LOAD (1 << 1)

# define PROCESS_STATE_ALIVE 0
# define PROCESS_STATE_ZOMBIE 1

# define PROCESS_CODE_SEGV 128

struct process
{
    uint8_t pid;

    int state;

    int exit_state;

    int type;

    struct as *as;

    size_t thread_count;

    struct klist threads;

    /* List of process */
    struct klist list;
};

void process_initialize(void);

struct process *process_create(int type, uintptr_t code, int flags);

int process_fork(struct process *process, struct irq_regs *regs);

void process_exit(struct process *p, int code);

void process_destroy(struct process *p);

#endif /* !PROCESS_H */

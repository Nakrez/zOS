#ifndef THREAD_H
# define THREAD_H

# include <kernel/types.h>
# include <kernel/klist.h>
# include <kernel/process.h>
# include <kernel/zos.h>
# include <kernel/interrupt.h>
# include <kernel/scheduler/event.h>

# include <arch/cpu.h>

# define THREAD_MAX_PER_PROCESS 10

# define THREAD_STATE_RUNNING 1
# define THREAD_STATE_BLOCKED 2
# define THREAD_STATE_ZOMBIE 3

struct thread
{
    int state;

    struct process *parent;

    size_t uid;
    size_t gid;

    int tid;

    int cpu;

    struct scheduler_event event;

    uint8_t interrupts[IRQ_USER_SIZE];

    uintptr_t kstack;

    struct thread_regs regs;

    /* List of thread in process */
    struct klist list;

    /* List for the scheduler */
    struct klist sched;
};

struct thread_glue
{
    int (*create)(struct process *, struct thread *, uintptr_t, size_t,
                  uintptr_t, uintptr_t);
    int (*duplicate)(struct thread *, struct irq_regs *);
    int (*current)(void);
    int (*save_state)(struct thread *, struct irq_regs *);
};

extern struct thread_glue thread_glue_dispatcher;

/*
 * Create a new thread inside a process
 */
int thread_create(struct process *process, uintptr_t code, size_t arg_count,
                  uintptr_t arg1, uintptr_t arg2);

int thread_duplicate(struct process *process, struct thread *thread,
                     struct irq_regs *regs);

/*
 * Get the running thread
 */
static inline struct thread *thread_current(void)
{
    return (struct thread *)glue_call(thread, current);
}

void thread_save_state(struct thread *thread, struct irq_regs *regs);

void thread_sleep(struct thread *thread, size_t ms, struct irq_regs *regs);

/*
 * Block a thread and remove it from scheduler
 *
 * The additionnal spinlock is unlock if l != NULL
 */
void thread_block(struct thread *thread, int event, int data, spinlock_t *l);
void thread_unblock(struct thread *thread);

void thread_exit(struct thread *thread);

void thread_destroy(struct thread *thread);

#endif /* !THREAD_H */

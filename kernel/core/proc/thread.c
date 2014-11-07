#include <string.h>

#include <kernel/console.h>
#include <kernel/panic.h>
#include <kernel/cpu.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/proc/thread.h>

#include <arch/mmu.h>

static int thread_new_tid(struct process *p)
{
    struct thread *thread;
    int used;

    for (int tid = 0; tid < THREAD_MAX_PER_PROCESS; ++tid)
    {
        used = 0;

        klist_for_each_elem(&p->threads, thread, list)
        {
            if (thread->tid == tid)
            {
                used = 1;
                break;
            }
        }

        if (!used)
            return tid;
    }

    return -1;
}

int thread_create(struct process *process, uintptr_t code, char *argv[],
                  int deep_argv_copy)
{
    int tid;
    struct thread *thread;

    if (!code)
        return -1;

    if ((tid = thread_new_tid(process)) < 0)
        return -1;

    /*
     * Allocate one entire page because thread structure is located on the
     * kernel stack
     */
    thread = (void *)(as_map(&kernel_as, 0, 0, PAGE_SIZE, AS_MAP_WRITE) +
             PAGE_SIZE - sizeof (struct thread));

    if (!thread)
        return -1;

    thread->parent = process;
    thread->tid = tid;

    thread->state = THREAD_STATE_RUNNING;

    thread->uid = 0;
    thread->gid = 0;
    thread->kstack = (uintptr_t)thread - 4;

    memset(thread->interrupts, 0, sizeof (thread->interrupts));
    memset(&thread->event, 0, sizeof (thread->event));

    if (!glue_call(thread, create, process, thread, code, argv,
                   deep_argv_copy))
    {
        as_unmap(&kernel_as, (vaddr_t)thread, AS_UNMAP_RELEASE);
        return -1;
    }

    ++process->thread_count;

    klist_add(&process->threads, &thread->list);

    cpu_add_thread(thread);

    return thread->tid;
}

int thread_update_exec(struct thread *thread, uintptr_t eip, char *argv[])
{
    thread->tid = 0;

    if (!glue_call(thread, create, thread->parent, thread, eip, argv, 1))
        return -1;

    return 0;
}

int thread_duplicate(struct process *process, struct thread *thread,
                     struct irq_regs *regs)
{
    int tid;
    struct thread *new;
    void *t_mem;

    if ((tid = thread_new_tid(process)) < 0)
        return 0;

    if (!(t_mem = (void *)(as_map(&kernel_as, 0, 0, PAGE_SIZE, AS_MAP_WRITE))))
        return 0;

    new = t_mem + PAGE_SIZE - sizeof (struct thread);

    new->parent = process;
    new->tid = tid;
    new->state = THREAD_STATE_RUNNING;

    new->uid = thread->uid;
    new->gid = thread->gid;
    new->kstack = (uintptr_t)new - 4;

    memset(thread->interrupts, 0, sizeof (thread->interrupts));
    memset(&thread->event, 0, sizeof (thread->event));

    if (!glue_call(thread, duplicate, new, regs))
    {
        as_unmap(&kernel_as, (vaddr_t)new, AS_UNMAP_RELEASE);

        return 0;
    }

    ++process->thread_count;

    klist_add(&process->threads, &new->list);

    cpu_add_thread(new);

    return 1;
}

void thread_save_state(struct thread *thread, struct irq_regs *regs)
{
    glue_call(thread, save_state, thread, regs);
}

static void timer_callback_sleep(int data)
{
    scheduler_event_notify(SCHED_EV_TIMER, data);
}

void thread_sleep(struct thread *thread, size_t ms, struct irq_regs *regs)
{
    uint32_t data = (thread->parent->pid << 16) | thread->tid;

    thread_save_state(thread, regs);

    timer_register(thread->cpu, TIMER_CALLBACK | TIMER_ONE_SHOT,
                   data, ms, timer_callback_sleep);

    thread_block(thread, SCHED_EV_TIMER, data, NULL);
}

void thread_block(struct thread *thread, int event, int data, spinlock_t *l)
{
    struct cpu *cpu = cpu_get(thread->cpu);

    thread->state = THREAD_STATE_BLOCKED;

    thread->event.event = event;
    thread->event.data = data;

    scheduler_event_wait(event, thread);

    if (l)
        spinlock_unlock(l);

    /*
     * Eventually we could be interrupted before this code and the thread
     * has been marked as blocked so the scheduler would already have removed
     * it. In that case at this point it would have been already unblocked
     * so we don't want to block it again
     */
    spinlock_lock(&cpu->scheduler.sched_lock);

    if (thread->state == THREAD_STATE_BLOCKED)
        scheduler_remove_thread(thread, &cpu->scheduler);
    else
        spinlock_unlock(&cpu->scheduler.sched_lock);
}

void thread_unblock(struct thread *thread)
{
    thread->state = THREAD_STATE_RUNNING;

    cpu_add_thread(thread);
}

void thread_exit(struct thread *thread)
{
    /* The thread will be destroy when it is elected by the scheduler */
    thread->state = THREAD_STATE_ZOMBIE;

    /* Unregister interrupts */
    for (int i = 0; i < IRQ_USER_SIZE; ++i)
    {
        if (thread->interrupts[i] & INTERRUPT_REGISTERED)
            interrupt_unregister(i + IRQ_USER_BEGIN);

        thread->interrupts[i] = 0;
    }
}

void thread_destroy(struct thread *thread)
{
    thread->kstack = align(thread->kstack, PAGE_SIZE) - PAGE_SIZE;

    klist_del(&thread->list);

    --(thread->parent->thread_count);

    if (!thread->parent->thread_count)
        process_destroy(thread->parent);

    as_unmap(&kernel_as, thread->kstack, AS_UNMAP_RELEASE);
}

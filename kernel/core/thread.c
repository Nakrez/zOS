#include <kernel/thread.h>
#include <kernel/kmalloc.h>
#include <kernel/console.h>
#include <kernel/cpu.h>
#include <kernel/panic.h>

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

int thread_create(struct process *process, uintptr_t code, size_t arg_count,
                  uintptr_t arg1, uintptr_t arg2)
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

    if (!glue_call(thread, create, process, thread, code, arg_count, arg1,
                   arg2))
    {
        as_unmap(&kernel_as, (vaddr_t)thread, AS_UNMAP_RELEASE);
        return -1;
    }

    ++process->thread_count;

    klist_add(&process->threads, &thread->list);

    cpu_add_thread(thread);

    return thread->tid;
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
    struct thread *thread = (void *)data;
    struct cpu *cpu = cpu_get(thread->cpu);

    thread->state = THREAD_STATE_RUNNING;

    scheduler_add_thread(&cpu->scheduler, thread);
}

void thread_sleep(struct thread *thread, size_t ms, struct irq_regs *regs)
{
    struct cpu *cpu = cpu_get(thread->cpu);

    thread_save_state(thread, regs);

    thread->state = THREAD_STATE_BLOCKED;

    timer_register(thread->cpu, TIMER_CALLBACK | TIMER_ONE_SHOT,
                   (int)thread, ms, timer_callback_sleep);

    scheduler_remove_thread(thread, &cpu->scheduler);
}

void thread_exit(struct thread *thread)
{
    struct cpu *cpu = cpu_get(cpu_id_get());

    /* The thread will be destroy when it is elected by the scheduler */
    thread->state = THREAD_STATE_ZOMBIE;

    if (cpu->scheduler.running == thread)
        cpu->scheduler.time = 1;

    scheduler_update(NULL);
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

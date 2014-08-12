#include <kernel/thread.h>
#include <kernel/kmalloc.h>
#include <kernel/console.h>
#include <kernel/cpu.h>

#include <arch/mmu.h>

int thread_create(struct process *process, uintptr_t code)
{
    struct thread *thread;

    if (!code)
        return 0;

    /*
     * Allocate one entire page because thread structure is located on the
     * kernel stack
     */
    thread = (void *)(as_map(&kernel_as, 0, 0, PAGE_SIZE, AS_MAP_WRITE) +
             PAGE_SIZE - sizeof (struct thread));

    if (!thread)
        return 0;

    thread->parent = process;

    thread->state = THREAD_STATE_RUNNING;

    thread->uid = 0;
    thread->gid = 0;
    thread->kstack = (uintptr_t)thread - 4;

    if (!_thread.create(process, thread, code))
    {
        as_unmap(&kernel_as, (vaddr_t)thread, AS_UNMAP_RELEASE);
        return 0;
    }

    klist_add(&process->threads, &thread->list);

    cpu_add_thread(thread);

    return 1;
}

static void timer_callback_sleep(int data)
{
    struct thread *thread = (void *)data;
    struct cpu *cpu = cpu_get(thread->cpu);

    scheduler_add_thread(&cpu->scheduler, thread);
}

void thread_sleep(struct thread *thread, size_t ms)
{
    struct cpu *cpu = cpu_get(thread->cpu);

    thread->state = THREAD_STATE_BLOCKED;

    timer_register(thread->cpu, TIMER_CALLBACK | TIMER_ONE_SHOT,
                   (int)thread, ms, timer_callback_sleep);

    scheduler_remove_thread(thread, &cpu->scheduler);
}

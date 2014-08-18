#include <kernel/scheduler.h>
#include <kernel/zos.h>
#include <kernel/console.h>
#include <kernel/cpu.h>

void scheduler_initialize(struct scheduler *sched)
{
    /*
     * Time = 0 so when enabling events the timer event will fire and first
     * thread will be scheduled
     */
    sched->time = 0;
    sched->running = NULL;
    sched->thread_num = 0;

    spinlock_init(&sched->sched_lock);

    klist_head_init(&sched->threads);
}

void scheduler_add_thread(struct scheduler *sched, struct thread *thread)
{
    spinlock_lock(&sched->sched_lock);

    ++sched->thread_num;

    klist_add_back(&sched->threads, &thread->sched);

    spinlock_unlock(&sched->sched_lock);
}

void scheduler_start(struct scheduler *sched)
{
    sched->time = 1;

    scheduler_update(NULL);
}

void scheduler_update(struct irq_regs *regs)
{
    struct cpu *cpu = cpu_get(cpu_id_get());

    --cpu->scheduler.time;

    if (cpu->scheduler.time <= 0)
    {
        struct thread *new_thread = scheduler_elect(&cpu->scheduler);

        while (new_thread->state == THREAD_STATE_ZOMBIE)
        {
            if (cpu->scheduler.running == new_thread)
            {
                cpu->scheduler.running = NULL;
                cpu->scheduler.time = 1;
            }

            thread_destroy(new_thread);

            new_thread = scheduler_elect(&cpu->scheduler);
        }

        if (new_thread != cpu->scheduler.running)
            scheduler_switch(&cpu->scheduler, new_thread, regs);
        else
            cpu->scheduler.time = SCHEDULER_TIME;
    }
}

struct thread *scheduler_elect(struct scheduler *sched)
{
    if (klist_empty(&sched->threads))
    {
        if (sched->running != NULL &&
            sched->running->state == THREAD_STATE_RUNNING)
            return sched->running;
        else
            return sched->idle;
    }

    /* Get first element in the thread list */
    struct thread *thread = klist_elem(sched->threads.next, struct thread,
                                       sched);

    klist_del(&thread->sched);

    return thread;
}

void scheduler_switch(struct scheduler *sched, struct thread *new_thread,
                      struct irq_regs *regs)
{
    struct thread *old = sched->running;

    sched->running = new_thread;
    sched->time = SCHEDULER_TIME;

    if (old != NULL)
        klist_add_back(&sched->threads, &old->sched);

    _scheduler.sswitch(regs, new_thread, old);
}

void scheduler_remove_thread(struct thread *t, struct scheduler *sched)
{
    if (t == sched->running)
    {
        sched->running = NULL;
        sched->time = 1;

        scheduler_update(NULL);
    }
    else
        klist_del(&t->sched);
}

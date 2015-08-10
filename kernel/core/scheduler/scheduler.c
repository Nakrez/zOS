#include <kernel/scheduler.h>
#include <kernel/zos.h>
#include <kernel/console.h>
#include <kernel/cpu.h>

void scheduler_initialize(struct scheduler *sched)
{
    /*
     * Time = 0 so when enabling interrupts the timer interrupt will fire and
     * first thread will be scheduled
     */
    sched->time = 0;
    sched->running = NULL;
    sched->thread_num = 0;

    spinlock_init(&sched->sched_lock);

    klist_head_init(&sched->threads);

    scheduler_event_initialize();
}

void scheduler_add_thread(struct scheduler *sched, struct thread *thread)
{
    spinlock_lock(&sched->sched_lock);

    ++sched->thread_num;

    klist_add_back(&sched->threads, &thread->sched);

    if (sched->running && sched->running == sched->idle)
        sched->time = 1;

    spinlock_unlock(&sched->sched_lock);
}

void scheduler_start(struct scheduler *sched)
{
    sched->time = 1;

    scheduler_update(NULL, 0);
}

static struct thread *scheduler_elect(struct scheduler *sched, int force)
{
    struct thread *thread;

    if (klist_empty(&sched->threads))
        return sched->idle;

    /* Get first element in the thread list */
    thread = klist_elem(sched->threads.next, struct thread, sched);

    klist_del(&thread->sched);

    if (force && sched->running == thread)
    {
        size_t count = 0;

        klist_for_each(&sched->threads, tmp, sched)
        {
            ++count;
        }

        klist_add_back(&sched->threads, &thread->sched);

        if (count == 0)
            thread = sched->idle;
        else
            thread = scheduler_elect(sched, force);
    }
    else
        klist_add_back(&sched->threads, &thread->sched);

    return thread;
}

static void scheduler_switch(struct scheduler *sched,
                             struct thread *new_thread, struct irq_regs *regs,
                             spinlock_t *sched_lock)
{
    struct thread *old = sched->running;

    sched->running = new_thread;
    sched->time = SCHEDULER_TIME;

    _scheduler.sswitch(regs, new_thread, old, sched_lock);
}

void scheduler_update(struct irq_regs *regs, int force)
{
    struct cpu *cpu = cpu_get(cpu_id_get());

    spinlock_lock(&cpu->scheduler.sched_lock);

    --cpu->scheduler.time;

    if (cpu->scheduler.time <= 0 ||
        cpu->scheduler.running->state != THREAD_STATE_RUNNING ||
        !regs || force ||
        (cpu->scheduler.running == cpu->scheduler.idle &&
         !klist_empty(&cpu->scheduler.threads)))
    {
        struct thread *thread;

        /* Clean blocked/zombie thread if any */
        klist_for_each(&cpu->scheduler.threads, tlist, sched)
        {
            thread = klist_elem(tlist, struct thread, sched);

            if (thread->state == THREAD_STATE_BLOCKED)
                klist_del(&thread->sched);
            else if (thread->state == THREAD_STATE_ZOMBIE)
            {
                if (cpu->scheduler.running == thread)
                    continue;

                klist_del(&thread->sched);

                /*
                 * FIXME: I don't like the fact that the lock is released here
                 * might cause bugs. Maybe an asynchronous event dispatcher
                 * would be cleaner and may improve stability a lot
                 * (thread_destroy dispatch two events which are responsable
                 * for dead locks if the lock is not released)
                 */
                spinlock_unlock_no_restore(&cpu->scheduler.sched_lock);

                thread_destroy(thread);

                spinlock_lock(&cpu->scheduler.sched_lock);
            }
        }

        thread = scheduler_elect(&cpu->scheduler, force);

        if (thread != cpu->scheduler.running)
            scheduler_switch(&cpu->scheduler, thread, regs,
                             &cpu->scheduler.sched_lock);
        else
            cpu->scheduler.time = SCHEDULER_TIME;

    }

    spinlock_unlock(&cpu->scheduler.sched_lock);
}

void scheduler_remove_thread(struct thread *t, struct scheduler *sched)
{
    /*
     * This function is only called by thread_block() which already lock the
     * scheduler lock
     */
    if (t->state == THREAD_STATE_RUNNING)
    {
        spinlock_unlock(&sched->sched_lock);
        return;
    }

    if (t == sched->running)
    {
        --sched->thread_num;

        spinlock_unlock(&sched->sched_lock);

        /* FIXME: Move that shit away ! */
        if (sched->running->state != THREAD_STATE_ZOMBIE)
        {
            __asm__ __volatile__("int $0x20");

            return;
        }

        sched->running = NULL;
        sched->time = 1;

        scheduler_update(NULL, 1);
    }
    else
    {
        --sched->thread_num;

        klist_del(&t->sched);

        spinlock_unlock(&sched->sched_lock);
    }
}

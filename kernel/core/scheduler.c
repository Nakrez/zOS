#include <kernel/scheduler.h>
#include <kernel/zos.h>
#include <kernel/console.h>

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

    klist_add(&sched->threads, &thread->sched);

    spinlock_unlock(&sched->sched_lock);
}

void scheduler_start(struct scheduler *sched)
{
    sched->time = SCHEDULER_TIME;
    sched->running = klist_elem(sched->threads.next, struct thread, sched);
}

void scheduler_update(struct irq_regs *regs)
{
    (void) regs;
}

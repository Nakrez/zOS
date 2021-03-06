#include <kernel/scheduler.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/proc/thread.h>

#include <arch/spinlock.h>
#include <arch/cpu.h>

static struct klist waiting_list[SCHED_EV_SIZE];
static spinlock_t event_lock;

void scheduler_event_initialize(void)
{
    spinlock_init(&event_lock);

    for (int i = 0; i < SCHED_EV_SIZE; ++i)
        klist_head_init(&waiting_list[i]);
}

void scheduler_event_notify(int event, int data)
{
    struct klist *ev_list;

    spinlock_lock(&event_lock);

    ev_list = &waiting_list[event - 1];

    klist_for_each(ev_list, wlist, block)
    {
        struct thread *waiting_thread;

        waiting_thread = klist_elem(wlist, struct thread, block);

        if (waiting_thread->event.event == event &&
            waiting_thread->event.data == data)
        {
            thread_unblock(waiting_thread);

            klist_del(&waiting_thread->block);
        }
    }

    spinlock_unlock(&event_lock);
}

void scheduler_event_wait(int event, struct thread *thread)
{
    spinlock_lock(&event_lock);
    klist_add(&waiting_list[event - 1], &thread->block);
    spinlock_unlock(&event_lock);
}

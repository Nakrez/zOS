#include <kernel/scheduler.h>
#include <kernel/kmalloc.h>

#include <arch/spinlock.h>

static spinlock_t waiting_lock = SPINLOCK_INIT;

static struct klist waiting_list = {
    &waiting_list,
    &waiting_list,
};

void scheduler_event_notify(int event, int data)
{
    spinlock_lock(&waiting_lock);

    klist_for_each(&waiting_list, wlist, list)
    {
        struct scheduler_wait *waiting;

        waiting = klist_elem(wlist, struct scheduler_wait, list);

        if (waiting->thread->event.event == event &&
            waiting->thread->event.data == data)
        {
            thread_unblock(waiting->thread);

            klist_del(&waiting->list);

            kfree(waiting);
        }
    }

    spinlock_unlock(&waiting_lock);
}

void scheduler_wait_event(struct thread *thread)
{
    /* TODO err */
    struct scheduler_wait *new = kmalloc(sizeof (struct scheduler_wait));

    new->thread = thread;

    spinlock_lock(&waiting_lock);

    klist_add(&waiting_list, &new->list);

    spinlock_unlock(&waiting_lock);
}

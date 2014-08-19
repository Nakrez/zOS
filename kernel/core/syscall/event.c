#include <kernel/syscall.h>
#include <kernel/thread.h>
#include <kernel/event.h>

static struct thread *events[IRQ_USER_SIZE];

static void user_event_callback(struct irq_regs *regs)
{
    int irq = regs->irq_num;
    struct thread *thread = events[irq - IRQ_USER_BEGIN];

    if (!thread)
        return;

    thread->events[irq - IRQ_USER_BEGIN] |= EVENT_FIRED;

    if (thread->state == THREAD_STATE_BLOCKED_EVENT)
        thread_unblock(thread);
}

int sys_event_register(struct syscall *interface)
{
    int event_id = interface->arg1;
    struct thread *t = thread_current();

    if (event_id < IRQ_USER_BEGIN || event_id > IRQ_USER_END)
        return -1;

    if (t->events[event_id - IRQ_USER_BEGIN] & EVENT_REGISTERED)
        return 0;

    t->events[event_id - IRQ_USER_BEGIN] = EVENT_REGISTERED;
    events[event_id - IRQ_USER_BEGIN] = t;

    if (!event_register(event_id, EVENT_CALLBACK, user_event_callback))
    {
        t->events[event_id - IRQ_USER_BEGIN] = 0;
        events[event_id - IRQ_USER_BEGIN] = NULL;
    }

    return 0;
}

int sys_event_listen(struct syscall *interface)
{
    (void) interface;

    int fired = -1;
    int registered = -1;

    struct thread *t = thread_current();

    for (int i = 0; i < IRQ_USER_SIZE; ++i)
    {
        if (t->events[i] & EVENT_REGISTERED)
            registered = 1;

        if (t->events[i] & EVENT_FIRED)
        {
            fired = i;
            break;
        }
    }

    /* Thread never registered any event */
    if (registered < 0)
        return -1;

    if (fired >= 0)
    {
        t->events[fired] = EVENT_REGISTERED;
        return fired + IRQ_USER_BEGIN;
    }

    /* Block thread until an event occured */
    thread_block(t, THREAD_STATE_BLOCKED_EVENT);

    for (int i = 0; i < IRQ_USER_SIZE; ++i)
    {
        if (t->events[i] & EVENT_FIRED)
        {
            t->events[i] = EVENT_REGISTERED;
            return i + IRQ_USER_BEGIN;
        }
    }

    return -1;
}

int sys_event_unregister(struct syscall *interface)
{
    int event_num = interface->arg1;
    struct thread *t = thread_current();

    if (event_num < IRQ_USER_BEGIN || event_num > IRQ_USER_END)
        return 0;

    if (!(t->events[event_num - IRQ_USER_BEGIN] & EVENT_REGISTERED))
        return 0;

    event_unregister(event_num);

    t->events[event_num - IRQ_USER_BEGIN] = 0;

    return 0;
}

#include <kernel/timer.h>
#include <kernel/panic.h>
#include <kernel/kmalloc.h>

static struct klist timers;

void timer_initialize(void)
{
    __timer.init();

    klist_head_init(&timers);
}

void timer_handler(int irq, int data)
{
    (void) irq;
    (void) data;

    struct timer_entry *timer;

    klist_for_each_elem(&timers, timer, list)
    {
        --timer->count;

        if (!timer->count)
        {
            if (timer->type & TIMER_CALLBACK)
                timer->callback(timer->data);
            else
                kernel_panic("Timer message not implemented");

            if (timer->type & TIMER_INFINITE)
                timer->count = timer->timer;
            else
            {
                klist_del(&timer->list);
                kfree(timer);
            }
        }
    }
}

int timer_register(int type, int data, size_t time, void (*callback)(int))
{
    struct timer_entry *timer;

    if (!(type & TIMER_CALLBACK) && !(type & TIMER_MESSAGE))
        return 0;

    if (!(type & TIMER_ONE_SHOT) && !(type & TIMER_INFINITE))
        return 0;

    if ((type & TIMER_CALLBACK) && !callback)
        return 0;

    if (!time)
        return 0;

    timer = kmalloc(sizeof (struct timer_entry));

    timer->type = type;
    timer->data = data;
    timer->timer = time / TIMER_GRANULARITY;
    timer->count = time / TIMER_GRANULARITY;
    timer->callback = callback;

    klist_add(&timers, &timer->list);

    return 1;
}

#include <kernel/zos.h>
#include <kernel/timer.h>
#include <kernel/panic.h>
#include <kernel/kmalloc.h>
#include <kernel/scheduler.h>
#include <kernel/cpu.h>

static struct timer_entry timers[TIMER_NUM];
spinlock_t timer_lock;

void timer_initialize(void)
{
    glue_call(timer, init);

    for (int i = 0; i < TIMER_NUM; ++i)
    {
        timers[i].free = 1;
    }

    spinlock_init(&timer_lock);
}

void timer_handler(struct irq_regs *regs)
{
    struct timer_entry *timer;
    struct cpu *cpu = cpu_get(cpu_id_get());

    klist_for_each(&cpu->timers, tlist, list)
    {
        timer = klist_elem(tlist, struct timer_entry, list);

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
                timer->free = 1;
            }
        }
    }

    scheduler_update(regs);
}

static int timer_new(void)
{
    for (int i = 0; i < TIMER_NUM; ++i)
        if (timers[i].free)
            return i;

    return -1;
}

int timer_register(int cpu_id, int type, int data, size_t time,
                   void (*callback)(int))
{
    int timer;
    struct cpu *cpu = cpu_get(cpu_id);

    if (!cpu)
        return 0;

    if (!(type & TIMER_CALLBACK) && !(type & TIMER_MESSAGE))
        return 0;

    if (!(type & TIMER_ONE_SHOT) && !(type & TIMER_INFINITE))
        return 0;

    if ((type & TIMER_CALLBACK) && !callback)
        return 0;

    if (!time)
        return 0;

    spinlock_lock(&timer_lock);

    timer = timer_new();

    if (timer < 0)
    {
        spinlock_unlock(&timer_lock);

        return 0;
    }

    timers[timer].free = 0;

    spinlock_unlock(&timer_lock);

    timers[timer].type = type;
    timers[timer].data = data;
    timers[timer].timer = time / TIMER_GRANULARITY;
    timers[timer].count = time / TIMER_GRANULARITY;
    timers[timer].callback = callback;

    klist_add(&cpu->timers, &timers[timer].list);

    return 1;
}

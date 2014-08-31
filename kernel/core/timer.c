#include <kernel/zos.h>
#include <kernel/timer.h>
#include <kernel/panic.h>
#include <kernel/kmalloc.h>
#include <kernel/scheduler.h>
#include <kernel/cpu.h>

void timer_initialize(void)
{
    glue_call(timer, init);
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
                kfree(timer);
            }
        }
    }

    scheduler_update(regs);
}

int timer_register(int cpu_id, int type, int data, size_t time,
                   void (*callback)(int))
{
    struct cpu *cpu = cpu_get(cpu_id);
    struct timer_entry *timer;

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

    timer = kmalloc(sizeof (struct timer_entry));

    timer->type = type;
    timer->data = data;
    timer->timer = time / TIMER_GRANULARITY;
    timer->count = time / TIMER_GRANULARITY;
    timer->callback = callback;

    klist_add(&cpu->timers, &timer->list);

    return 1;
}

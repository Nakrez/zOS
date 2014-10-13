#include <kernel/syscall.h>
#include <kernel/interrupt.h>

#include <kernel/proc/thread.h>

#include <kernel/scheduler/event.h>

static struct thread *interrupts[IRQ_USER_SIZE];

static void user_interrupt_callback(struct irq_regs *regs)
{
    int irq = regs->irq_num;
    struct thread *thread = interrupts[irq - IRQ_USER_BEGIN];

    if (!thread)
        return;

    thread->interrupts[irq - IRQ_USER_BEGIN] |= INTERRUPT_FIRED;

    scheduler_event_notify(SCHED_EV_INTERRUPT, irq);
}

int sys_interrupt_register(struct syscall *interface)
{
    int interrupt_id = interface->arg1;
    struct thread *t = thread_current();

    if (interrupt_id < IRQ_USER_BEGIN || interrupt_id > IRQ_USER_END)
        return -1;

    if (t->interrupts[interrupt_id - IRQ_USER_BEGIN] & INTERRUPT_REGISTERED)
        return 0;

    t->interrupts[interrupt_id - IRQ_USER_BEGIN] = INTERRUPT_REGISTERED;
    interrupts[interrupt_id - IRQ_USER_BEGIN] = t;

    if (!interrupt_register(interrupt_id, INTERRUPT_CALLBACK,
                            user_interrupt_callback))
    {
        t->interrupts[interrupt_id - IRQ_USER_BEGIN] = 0;
        interrupts[interrupt_id - IRQ_USER_BEGIN] = NULL;
    }

    return 0;
}

int sys_interrupt_listen(struct syscall *interface)
{
    struct thread *t = thread_current();

    if (interface->arg1 < IRQ_USER_BEGIN || interface->arg1 > IRQ_USER_END)
        return -1;

    if (!(t->interrupts[interface->arg1 - IRQ_USER_BEGIN] &
          INTERRUPT_REGISTERED))
        return -1;

    if (t->interrupts[interface->arg1 - IRQ_USER_BEGIN] & INTERRUPT_FIRED)
        return interface->arg1;

    /* Block thread until an interrupt occured */
    thread_block(t, SCHED_EV_INTERRUPT, interface->arg1, NULL);

    if (t->interrupts[interface->arg1 - IRQ_USER_BEGIN] & INTERRUPT_FIRED)
        return interface->arg1;

    return -1;
}

int sys_interrupt_unregister(struct syscall *interface)
{
    int interrupt_num = interface->arg1;
    struct thread *t = thread_current();

    if (interrupt_num < IRQ_USER_BEGIN || interrupt_num > IRQ_USER_END)
        return 0;

    if (!(t->interrupts[interrupt_num - IRQ_USER_BEGIN] &
          INTERRUPT_REGISTERED))
        return 0;

    interrupt_unregister(interrupt_num);

    t->interrupts[interrupt_num - IRQ_USER_BEGIN] = 0;

    return 0;
}

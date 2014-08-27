#include <kernel/syscall.h>
#include <kernel/thread.h>
#include <kernel/interrupt.h>

static struct thread *interrupts[IRQ_USER_SIZE];

static void user_interrupt_callback(struct irq_regs *regs)
{
    int irq = regs->irq_num;
    struct thread *thread = interrupts[irq - IRQ_USER_BEGIN];

    if (!thread)
        return;

    thread->interrupts[irq - IRQ_USER_BEGIN] |= INTERRUPT_FIRED;

    if (thread->state == THREAD_STATE_BLOCKED_INTERRUPT)
        thread_unblock(thread);
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
    (void) interface;

    int fired = -1;
    int registered = -1;

    struct thread *t = thread_current();

    for (int i = 0; i < IRQ_USER_SIZE; ++i)
    {
        if (t->interrupts[i] & INTERRUPT_REGISTERED)
            registered = 1;

        if (t->interrupts[i] & INTERRUPT_FIRED)
        {
            fired = i;
            break;
        }
    }

    /* Thread never registered any interrupt */
    if (registered < 0)
        return -1;

    if (fired >= 0)
    {
        t->interrupts[fired] = INTERRUPT_REGISTERED;
        return fired + IRQ_USER_BEGIN;
    }

    /* Block thread until an interrupt occured */
    thread_block(t, THREAD_STATE_BLOCKED_INTERRUPT);

    for (int i = 0; i < IRQ_USER_SIZE; ++i)
    {
        if (t->interrupts[i] & INTERRUPT_FIRED)
        {
            t->interrupts[i] = INTERRUPT_REGISTERED;
            return i + IRQ_USER_BEGIN;
        }
    }

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

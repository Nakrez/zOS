#include <kernel/errno.h>
#include <kernel/syscall.h>
#include <kernel/interrupt.h>

#include <kernel/proc/thread.h>
#include <kernel/proc/wait_queue.h>

#include <kernel/scheduler/event.h>

struct irq {
    struct thread *thread;

    struct wait_queue queue;
};

static struct irq interrupts[IRQ_USER_SIZE];

static void user_interrupt_callback(struct irq_regs *regs)
{
    int irq_num = regs->irq_num - IRQ_USER_BEGIN;
    struct irq *irq = &interrupts[irq_num];

    if (!irq->thread)
        return;

    irq->thread->interrupts[irq_num] |= INTERRUPT_FIRED;

    wait_queue_notify(&irq->queue);
}

int sys_interrupt_register(struct syscall *interface)
{
    int err;
    int irq_num = interface->arg1;
    int user_irq = irq_num - IRQ_USER_BEGIN;
    struct irq *irq;
    struct thread *t = thread_current();

    if (irq_num < IRQ_USER_BEGIN || irq_num > IRQ_USER_END)
        return -EINVAL;

    if (t->interrupts[user_irq] & INTERRUPT_REGISTERED)
        return -EBUSY;

    irq = &interrupts[user_irq];

    t->interrupts[user_irq] = INTERRUPT_REGISTERED;
    irq->thread = t;
    wait_queue_init(&irq->queue);

    err = interrupt_register(irq_num, INTERRUPT_CALLBACK,
                             user_interrupt_callback);
    if (err < 0) {
        t->interrupts[user_irq] = 0;
        irq->thread = NULL;
        return err;
    }

    return 0;
}

int sys_interrupt_listen(struct syscall *interface)
{
    int irq_num = interface->arg1;
    int user_irq = irq_num - IRQ_USER_BEGIN;
    struct irq *irq;
    struct thread *t = thread_current();

    if (irq_num < IRQ_USER_BEGIN || irq_num > IRQ_USER_END)
        return -1;

    if (!(t->interrupts[user_irq] & INTERRUPT_REGISTERED))
        return -1;

    if (t->interrupts[user_irq] & INTERRUPT_FIRED) {
        t->interrupts[user_irq] &= ~INTERRUPT_FIRED;
        return irq_num;
    }

    irq = &interrupts[user_irq];

    /* Block thread until an interrupt occured */
    wait_queue_wait(&irq->queue, t, t->interrupts[user_irq] & INTERRUPT_FIRED);

    t->interrupts[user_irq] &= ~INTERRUPT_FIRED;

    return irq_num;
}

int sys_interrupt_unregister(struct syscall *interface)
{
    int irq_num = interface->arg1;
    int user_irq = irq_num - IRQ_USER_BEGIN;
    struct thread *t = thread_current();

    if (irq_num < IRQ_USER_BEGIN || irq_num > IRQ_USER_END)
        return 0;

    if (!(t->interrupts[user_irq] & INTERRUPT_REGISTERED))
        return 0;

    interrupt_unregister(irq_num);

    t->interrupts[user_irq] = 0;
    interrupts[user_irq].thread = NULL;

    return 0;
}

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
    struct irq *irq;
    struct thread *t = thread_current();

    if (irq_num < IRQ_USER_BEGIN || irq_num > IRQ_USER_END)
        return -EINVAL;

    irq_num -= IRQ_USER_BEGIN;

    if (t->interrupts[irq_num] & INTERRUPT_REGISTERED)
        return -EBUSY;

    irq = &interrupts[irq_num];

    t->interrupts[irq_num] = INTERRUPT_REGISTERED;
    irq->thread = t;
    wait_queue_init(&irq->queue);

    err = interrupt_register(irq_num, INTERRUPT_CALLBACK,
                             user_interrupt_callback);
    if (err < 0) {
        t->interrupts[irq_num] = 0;
        irq->thread = NULL;
        return err;
    }

    return 0;
}

int sys_interrupt_listen(struct syscall *interface)
{
    int irq_num = interface->arg1;
    struct thread *t = thread_current();

    if (irq_num < IRQ_USER_BEGIN || irq_num > IRQ_USER_END)
        return -1;

    irq_num -= IRQ_USER_BEGIN;

    if (!(t->interrupts[irq_num] & INTERRUPT_REGISTERED))
        return -1;

    if (t->interrupts[irq_num] & INTERRUPT_FIRED) {
        t->interrupts[irq_num] &= ~INTERRUPT_FIRED;

        return interface->arg1;
    }

    /* Block thread until an interrupt occured */
    thread_block(t, SCHED_EV_INTERRUPT, interface->arg1, NULL);

    return interface->arg1;
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
    interrupts[interrupt_num - IRQ_USER_BEGIN].thread = NULL;

    return 0;
}

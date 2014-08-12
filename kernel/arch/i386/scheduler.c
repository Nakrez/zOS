#include <kernel/panic.h>
#include <kernel/cpu.h>

#include <arch/scheduler.h>
#include <arch/pm.h>

static void save_thread(struct irq_regs *regs, struct thread *thread)
{
    if (!thread || !regs)
        return;

    thread->regs.gs = regs->gs;
    thread->regs.fs = regs->fs;
    thread->regs.es = regs->es;
    thread->regs.ds = regs->ds;

    thread->regs.edi = regs->edi;
    thread->regs.esi = regs->esi;
    thread->regs.ebp = regs->ebp;
    thread->regs.ebx = regs->ebx;
    thread->regs.edx = regs->edx;
    thread->regs.ecx = regs->ecx;
    thread->regs.eax = regs->eax;

    thread->regs.eip = regs->eip;
    thread->regs.cs = regs->cs;
    thread->regs.eflags = regs->eflags;

    if (thread->regs.cs != KERNEL_CS)
    {
        thread->regs.esp = regs->user_esp;
        thread->regs.ss = regs->user_ss;
    }
    else
    {
        /*
         * We want esp to point before irq_regs since the user process was in
         * ring0, probably for a syscall, ss and esp were not pushed
         */
        thread->regs.esp = (uintptr_t)regs + sizeof (struct irq_regs) - 8;
        thread->regs.ss = KERNEL_DS;
    }
}

void i386_switch(struct irq_regs *regs, struct thread *new,
                 struct thread *old)
{
    struct process *parent = new->parent;
    struct cpu *cpu = cpu_get(cpu_id_get());

    save_thread(regs, old);

    if (new->regs.cs == KERNEL_CS)
        __asm__ __volatile__("mov %0, %%esp\n"
                             :
                             : "r" (new->regs.esp));

    /* Set kernel stack as esp0 */
    cpu->arch.tss.esp0 = new->kstack;

    cr3_set(parent->as->arch.cr3);

    ds_set(new->regs.ds);
    es_set(new->regs.es);
    fs_set(new->regs.fs);
    gs_set(new->regs.gs);

    __asm__ __volatile__("cmp %5, %1\n"
                         "jz 1f\n"
                         "push %4\n"
                         "push %3\n"
                         "1:\n"
                         "push %0\n"
                         "push %1\n"
                         "push %2\n"
                         "push %6\n"
                         "push %7\n"
                         "push %8\n"
                         "push %9\n"
                         "pushl $0\n"
                         "push %10\n"
                         "push %11\n"
                         "push %12\n"
                         "popal\n"
                         "iret\n"
                         :
                         : "g" (new->regs.eflags),
                           "g" (new->regs.cs),
                           "g" (new->regs.eip),
                           "g" (new->regs.esp),
                           "g" (new->regs.ss),
                           "i" (KERNEL_CS),
                           "g" (new->regs.eax),
                           "g" (new->regs.ecx),
                           "g" (new->regs.edx),
                           "g" (new->regs.ebx),
                           "g" (new->regs.ebp),
                           "g" (new->regs.esi),
                           "g" (new->regs.edi)
                        : "memory");
}

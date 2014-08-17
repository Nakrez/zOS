#include <kernel/zos.h>
#include <kernel/region.h>
#include <kernel/panic.h>

#include <arch/thread.h>
#include <arch/pm.h>
#include <arch/mmu.h>

int i386_thread_create(struct process *p, struct thread *t, uintptr_t eip)
{
    if (p->type == PROCESS_TYPE_KERNEL)
    {
        t->regs.cs = KERNEL_CS;
        t->regs.ds = KERNEL_DS;
        t->regs.es = KERNEL_DS;
        t->regs.fs = KERNEL_DS;
        t->regs.gs = KERNEL_DS;
        t->regs.ss = KERNEL_DS;

        t->regs.esp = t->kstack;
    }
    else
    {
        t->regs.cs = USER_CS;
        t->regs.ds = USER_DS;
        t->regs.es = USER_DS;
        t->regs.fs = USER_DS;
        t->regs.gs = USER_DS;
        t->regs.ss = USER_DS;

        t->regs.esp = region_reserve(p->as, 0xC0000000 - 2 * PAGE_SIZE, 1);

        if (!t->regs.esp)
            return 0;

        /* Reserve user stack */
        t->regs.esp = as_map(p->as, t->regs.esp, 0, PAGE_SIZE,
                             AS_MAP_WRITE | AS_MAP_USER);

        if (!t->regs.esp)
        {
            as_unmap(p->as, t->regs.esp, 0);
            return 0;
        }

        t->regs.esp += PAGE_SIZE - 4;
    }

    t->regs.edi = 0;
    t->regs.esi = 0;
    t->regs.ebp = 0;
    t->regs.__esp = 0;
    t->regs.ebx = 0;
    t->regs.edx = 0;
    t->regs.ecx = 0;
    t->regs.eax = 0;
    t->regs.eip = eip;
    t->regs.eflags = 0x200;

    return 1;
}

int i386_thread_duplicate(struct thread *thread, struct irq_regs *regs)
{
    if (regs->eip > KERNEL_BEGIN)
        kernel_panic("Fork new process eip is in kernel land... Humm ... :(");

    if (regs->cs == KERNEL_CS)
        kernel_panic("Forking kernel process... Funny :)");

    thread->regs.cs = regs->cs;
    thread->regs.ds = regs->ds;
    thread->regs.es = regs->es;
    thread->regs.fs = regs->fs;
    thread->regs.gs = regs->gs;
    thread->regs.ss = regs->user_ss;

    thread->regs.edi = regs->edi;
    thread->regs.esi = regs->esi;
    thread->regs.ebp = regs->ebp;
    thread->regs.__esp = 0;
    thread->regs.esp = regs->user_esp;
    thread->regs.ebx = regs->ebx;
    thread->regs.edx = regs->edx;
    thread->regs.ecx = regs->ecx;
    thread->regs.eip = regs->eip;
    thread->regs.eflags = regs->eflags;

    /* Called by fork, configuration the son, must return 0 */
    thread->regs.eax = 0;

    return 1;
}

int i386_thread_current(void)
{
    uint32_t esp;

    __asm__ __volatile__("mov %%esp, %0"
                         : "=r" (esp)
                         :);

    /* Special case for not yet initialized kernel. It is used by as_map */
    if (esp < 0xC0400000)
        return 0;

    esp = align(esp, PAGE_SIZE) - sizeof (struct thread);

    return esp;
}

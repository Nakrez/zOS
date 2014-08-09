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

        /* Reserve user stack */
        t->regs.esp = as_map(p->as, 0xC0000000 - PAGE_SIZE, 0, PAGE_SIZE,
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

    return 1;
}

struct thread *i386_thread_current(void)
{
    uint32_t esp;

    __asm__ __volatile__("mov %%esp, %0"
                         : "=r" (esp)
                         :);

    esp = ((esp + PAGE_SIZE) & PAGE_SIZE) - sizeof (struct thread);

    return (struct thread *)esp;
}

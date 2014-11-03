#include <string.h>

#include <kernel/zos.h>
#include <kernel/panic.h>

#include <kernel/mem/region.h>
#include <kernel/mem/segment.h>

#include <arch/thread.h>
#include <arch/pm.h>
#include <arch/mmu.h>

/* FIXME: Disgusting */
static int i386_setup_stack(char *stack, paddr_t phy_stack, char *argv[],
                            int deep_argv_copy)
{
    /* Stack layout
     * -------------
     * -- TLS
     * -- argv content --
     * -- argv[n] --
     * -- ....... --
     * -- argv[0] --
     * --  argc   --
     */
    int argc = 0;
    const char *stack_base = stack;
    uintptr_t *stack_ptr;
    char *kstack = (void *)as_map(&kernel_as, 0, (paddr_t)phy_stack, PAGE_SIZE,
                                  AS_MAP_WRITE);
    char *argv_start;

    if (!kstack)
        return 1;

    kstack += (PAGE_SIZE - 4) & 0xFFFFFFF0;

    /* Reserve space for TLS Pointer */
    kstack -= 2 * sizeof (uintptr_t);
    stack -= 2 * sizeof (uintptr_t);

    if (argv)
    {
        /* Copy argv content */
        for (; argv[argc]; ++argc)
        {
            if (deep_argv_copy)
            {
                int len = strlen(argv[argc]) + 1;

                kstack -= len;
                stack -= len;

                memcpy(kstack, argv[argc], len);

                argv[argc] = stack;

                if (len % 16)
                {
                    stack -= 16 - (len % 16);
                    kstack -= 16 - (len % 16);
                }
            }
        }

        if (deep_argv_copy)
        {
            stack -= 16;
            kstack -= 16;
        }
    }

    stack_ptr = (void *)kstack;

    *(stack_ptr--) = 0;
    stack -= sizeof (uintptr_t);

    /* Put argv table pointer */

    if (argc > 0)
    {
        for (int i = argc - 1; i >= 0; --i)
        {
            *(stack_ptr--) = (uintptr_t)argv[i];
            stack -= sizeof (uintptr_t);
        }
    }

    argv_start = stack + sizeof (uintptr_t);

    kstack = (void *)stack_ptr;

    if (deep_argv_copy)
    {
        while (((uintptr_t)stack - sizeof (uintptr_t)) % 16)
        {
            --stack;
            --kstack;
        }

        stack_ptr = (void *)kstack;
    }
    else
    {
        while (((uintptr_t)stack - 2 * sizeof (uintptr_t)) % 16)
        {
            --stack;
            --kstack;
        }

        stack_ptr = (void *)kstack;
    }

    stack_ptr = (void *)kstack;

    /* Argv address */
    *(stack_ptr--) = (uintptr_t)argv_start;
    stack -= sizeof (uintptr_t);

    /* Put argc */
    if (deep_argv_copy)
        *(stack_ptr) = argc;
    else
    {
        *(stack_ptr--) = (uintptr_t)argc;
        stack -= sizeof (uintptr_t);
    }

    as_unmap(&kernel_as, (vaddr_t)kstack, AS_UNMAP_NORELEASE);

    return stack_base - stack;
}

int i386_thread_create(struct process *p, struct thread *t, uintptr_t eip,
                       char *argv[], int deep_argv_copy)
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
        paddr_t phy_stack;

        t->regs.cs = USER_CS;
        t->regs.ds = USER_DS;
        t->regs.es = USER_DS;
        t->regs.fs = USER_DS;
        t->regs.gs = USER_DS;
        t->regs.ss = USER_DS;

        t->regs.esp = region_reserve(p->as, THREAD_STACK_BASE -
                                     t->tid * THREAD_MAX_STACK_SIZE,
                                     1);

        if (!t->regs.esp)
            return 0;

        phy_stack = segment_alloc_address(1);

        /* TODO: Release region */
        if (!phy_stack)
            return 0;

        /* Reserve user stack */
        t->regs.esp = as_map(p->as, t->regs.esp, phy_stack, PAGE_SIZE,
                             AS_MAP_WRITE | AS_MAP_USER);

        if (!t->regs.esp)
        {
            as_unmap(p->as, t->regs.esp, 0);
            return 0;
        }

        t->regs.esp += (PAGE_SIZE - 4) & 0xFFFFFFF0;

        t->regs.esp -= (uintptr_t)i386_setup_stack((void *)t->regs.esp,
                                                   phy_stack, argv,
                                                   deep_argv_copy);
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

int i386_thread_save_state(struct thread *thread, struct irq_regs *regs)
{
    if (!thread || !regs)
        return 1;

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

    return 1;
}

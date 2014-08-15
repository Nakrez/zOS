#include <kernel/panic.h>
#include <kernel/console.h>
#include <kernel/thread.h>
#include <kernel/process.h>

#include <arch/page_fault.h>

void page_fault_handler(struct irq_regs *regs)
{
    struct thread *thread = thread_current();
    vaddr_t addr_fault = cr2_get();

    if (!thread)
    {
        console_message(T_ERR, "Page fault happening without a running "
                        "thread");
        console_message(T_ERR, "This must be bad, I can't do anything sorry");
        console_message(T_ERR, "Faulting address: 0x%x", addr_fault);
        console_message(T_ERR, "Error code: 0x%x", regs->irq_data);

        kernel_panic("");
    }

    if (regs->irq_data & PAGE_FAULT_USER)
    {
        if (regs->irq_data & PAGE_FAULT_WRITE)
            kernel_panic("page_fault_handler: check COW needed");
        else
        {
            console_message(T_ERR, "Process %i: page fault");

            process_exit(thread->parent, PROCESS_CODE_SEGV);
        }
    }
    else
    {
        console_message(T_ERR, "KERNEL PAGE FAULT");

        console_message(T_ERR, "Faulting address: 0x%x", addr_fault);
        console_message(T_ERR, "Error code: 0x%x", regs->irq_data);

        kernel_panic("");
    }
}

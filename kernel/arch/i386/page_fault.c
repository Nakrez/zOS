#include <kernel/panic.h>
#include <kernel/console.h>
#include <kernel/thread.h>
#include <kernel/process.h>
#include <kernel/segment.h>

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
        {
            struct as_mapping *mapping;

            mapping = as_mapping_locate(thread->parent->as, addr_fault);

            if (mapping && mapping->phy->flags & SEGMENT_FLAGS_COW)
            {
                kernel_panic("page_fault_handler: user COW implem needed");

                return;
            }
        }

        console_message(T_ERR, "Process %i: page fault", thread->parent->pid);

        process_exit(thread->parent, PROCESS_CODE_SEGV);
    }
    else
    {
        /* Check if a COW is needed (for page table) */
        if (regs->irq_data & PAGE_FAULT_WRITE && addr_fault >= 0xFFC00000)
        {
            struct segment *seg;
            paddr_t pt = as_virt_to_phy(addr_fault);

            seg = segment_locate(pt);

            if (seg && seg->flags & SEGMENT_FLAGS_COW)
                kernel_panic("page_fault_handler: kernel COW implem needed");
        }

        console_message(T_ERR, "KERNEL PAGE FAULT");

        console_message(T_ERR, "Faulting address: 0x%x", addr_fault);
        console_message(T_ERR, "Error code: 0x%x", regs->irq_data);

        kernel_panic("");
    }
}

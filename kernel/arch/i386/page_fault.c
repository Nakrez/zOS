#include <string.h>

#include <kernel/zos.h>
#include <kernel/panic.h>
#include <kernel/console.h>

#include <kernel/mem/segment.h>

#include <kernel/proc/thread.h>
#include <kernel/proc/process.h>

#include <arch/page_fault.h>
#include <arch/mmu.h>

static void cow(struct as *as, struct as_mapping *mapping)
{
    if (mapping->phy->ref_count == 1)
        mapping->phy->flags &= ~SEGMENT_FLAGS_COW;
    else
    {
        /* Allocate new physical memory */
        struct segment *seg = segment_alloc(mapping->size / PAGE_SIZE);

        uint32_t *new_page;
        uint32_t *old_page = (uint32_t *)mapping->virt;

        if (!seg)
            kernel_panic("COW: Out of memory");

        /* Copy memory area */
        new_page = (uint32_t *)as_map(&kernel_as, 0, seg->base, mapping->size,
                                      AS_MAP_WRITE);

        if (!new_page)
            kernel_panic("COW: Unable to copy");

        memcpy(new_page, old_page, mapping->size);

        as_unmap(&kernel_as, (vaddr_t)new_page, AS_UNMAP_NORELEASE);

        /* Since we duplicated the pages there is one less reference */
        segment_release(mapping->phy);

        mapping->phy = seg;
    }

    /* Remap with old rights */
    if (!as_remap(as, mapping, mapping->flags))
        kernel_panic("COW: unable to remap memory area");
}

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

    if (addr_fault < KERNEL_BEGIN || regs->irq_data & PAGE_FAULT_USER)
    {
        if (regs->irq_data & PAGE_FAULT_WRITE)
        {
            struct as_mapping *mapping;

            mapping = as_mapping_locate(thread->parent->as, addr_fault);

            if (mapping && mapping->phy->flags & SEGMENT_FLAGS_COW)
            {
                cow(thread->parent->as, mapping);

                return;
            }
        }

        if (regs->irq_data & PAGE_FAULT_USER)
        {
            console_message(T_ERR, "Process (%i, %i): page fault (0x%x, 0x%x)",
                            thread->parent->pid, thread->tid, addr_fault,
                            regs->irq_data);

            process_exit(thread->parent, PROCESS_CODE_SEGV);

            return;
        }
    }

    console_message(T_ERR, "KERNEL PAGE FAULT");

    console_message(T_ERR, "Faulting address: 0x%x", addr_fault);
    console_message(T_ERR, "Error code: 0x%x", regs->irq_data);

    kernel_panic("");
}

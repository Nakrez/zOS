#include <string.h>

#include <kernel/as.h>
#include <kernel/segment.h>
#include <kernel/panic.h>

#include <arch/mmu.h>
#include <arch/cpu.h>

int mmu_init_kernel(struct as *as)
{
    /* cr3 is mapped at 0xC0000000 by the bootloader */
    uint32_t *kpd = (uint32_t *) 0xC0000000;

    /* 0xC0001000 is safe to use for initial page table */
    uint32_t *kpt = (uint32_t *) 0xC0001000;

    paddr_t kpt_phy = 0x1000;

    /* Set cr3 */
    as->arch.cr3 = 0x0;

    /* Map kernel in the page table */
    for (size_t i = 0; i < PAGE_SIZE; ++i)
        kpt[i] = (i * PAGE_SIZE) | PT_PRESENT | PT_WRITE;

    /* Clean page directory */
    memset(kpd, 0, PAGE_SIZE);

    kpd[768] = kpt_phy | PD_PRESENT | PD_WRITE;

    /* Map extended kernel heap page table */
    for (size_t i = KERNEL_PT_START_INDEX; i < KERNEL_PT_END_INDEX; ++i)
        kpd[767 + i] = i * PAGE_SIZE | PD_PRESENT | PD_WRITE;

    /* Mirroring setup */
    kpd[1023] = as->arch.cr3 | PD_PRESENT | PD_WRITE;

    cpu_flush_tlb();

    return 1;
}

int mmu_init_user(struct as *as)
{
    (void) as;

    return 1;
}

static int as_to_mmu_flags(int flags)
{
    int mmu_flags = PT_PRESENT;

    if (flags & AS_MAP_USER)
        flags |= PT_USER;

    if (flags & AS_MAP_WRITE)
        flags |= PT_WRITE;

    /*
     * FIXME: AS_MAP_EXEC is discard because no software exec solution is
     * implemented
     */

    return mmu_flags;
}

int install_pt_if_needed(uint32_t *pd, uint32_t pd_index, int flags)
{
    if (!(pd[pd_index] & PD_PRESENT))
    {
        /* Allocate 1 physical page for the new page table */
        paddr_t phy_pt = segment_alloc(1);

        if (!phy_pt)
            return 0;

        /* We can use flags because PD and PT flags are the same */
        pd[pd_index] = phy_pt | flags;
    }

    return 1;
}

int mmu_map(struct as *as, vaddr_t vaddr, paddr_t paddr, size_t size,
            int flags)
{
    (void) as;

    uint32_t pd_index = (vaddr >> 22) & 0x3FF;
    uint32_t pt_index = (vaddr >> 12) & 0x3FF;

    uint32_t number_of_page = size / PAGE_SIZE;

    uint32_t *pd = (uint32_t *)0xFFFFF000;
    uint32_t *pt = (uint32_t *)(0xFFC00000 + 0x1000 * pd_index);

    int mmu_flags = as_to_mmu_flags(flags);

    if (!install_pt_if_needed(pd, pd_index, mmu_flags))
        return 0;

    while (1)
    {
        pt[pt_index] = paddr | mmu_flags;

        paddr += PAGE_SIZE;

        --number_of_page;

        if (!number_of_page)
            break;

        ++pt_index;

        /* Mapping overlap between 2 pts */
        if (pt_index > 1023)
        {
            ++pd_index;
            pt_index = 0;

            /* FIXME: unmap what has already been mapped */
            if (!install_pt_if_needed(pd, pd_index, flags))
                return 0;

            pt = (uint32_t *)(0xFFC00000 + 0x1000 * pd_index);
        }
    }

    return 1;
}

void mmu_unmap(struct as *as, vaddr_t vaddr, size_t size)
{
    (void) as;
    (void) vaddr;
    (void) size;
}

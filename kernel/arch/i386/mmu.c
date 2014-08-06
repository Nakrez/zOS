#include <string.h>

#include <kernel/as.h>

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

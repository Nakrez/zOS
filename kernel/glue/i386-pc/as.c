#include <kernel/panic.h>

#include <kernel/mem/as.h>

#include <glue/as.h>

#include <arch/mmu.h>

struct as_glue as_glue_dispatcher =
{
    i386_pc_as_initialize,
    mmu_map,
    mmu_unmap,
    mmu_duplicate,
    mmu_virt_to_phy,
    i386_pc_as_destroy,
};

int i386_pc_as_initialize(struct as *as)
{
    if (as == &kernel_as)
        return mmu_init_kernel(as);

    return mmu_init_user(as);
}

int i386_pc_as_destroy(struct as *as)
{
    if (as == &kernel_as)
        kernel_panic("Attempting to destroy kernel address space are you mad "
                     "?");

    mmu_remove_cr3(as);

    return 1;
}

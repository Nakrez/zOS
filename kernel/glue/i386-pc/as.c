#include <kernel/as.h>

#include <glue/as.h>

#include <arch/mmu.h>

struct as_glue __as =
{
    i386_pc_as_initialize,
    mmu_map,
    mmu_unmap,
};

int i386_pc_as_initialize(struct as *as)
{
    if (as == &kernel_as)
        return mmu_init_kernel(as);

    return mmu_init_user(as);
}

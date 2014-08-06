#include <kernel/as.h>

#include <glue/as.h>

#include <arch/mmu.h>

struct as_glue __as =
{
    i386_pc_as_initialize,
    i386_pc_as_map,
    i386_pc_as_unmap,
};

int i386_pc_as_initialize(struct as *as)
{
    if (as == &kernel_as)
        return mmu_init_kernel(as);

    return mmu_init_user(as);
}

int i386_pc_as_map(vaddr_t vaddr, paddr_t paddr, size_t size, int flags)
{

}

void i386_pc_as_unmap(vaddr_t vaddr, size_t size)
{

}

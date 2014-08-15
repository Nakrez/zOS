#include <kernel/as.h>
#include <kernel/zos.h>
#include <kernel/kmalloc.h>
#include <kernel/region.h>
#include <kernel/segment.h>

#include <arch/mmu.h>

struct as kernel_as;

struct as *as_create(void)
{
    struct as *as = kmalloc(sizeof (struct as));

    if (!as_initialize(as))
    {
        kfree(as);

        return NULL;
    }

    return as;
}

int as_initialize(struct as* as)
{
    if (!glue_call(as, init, as))
        return 0;

    spinlock_init(&as->region_lock);
    spinlock_init(&as->map_lock);

    region_initialize(as);

    klist_head_init(&as->mapping);

    /* Add the initial kernel mapping */
    if (as == &kernel_as)
    {
        struct as_mapping *map;

        map = kmalloc(sizeof (struct as_mapping));

        map->virt = KERNEL_BEGIN;
        /* FIXME: Arch define or move initial mapping alloc to arch code */
        map->phy = 0;
        map->size = 1024 * PAGE_SIZE;

        klist_add(&as->mapping, &map->list);
    }

    return 1;
}

vaddr_t as_map(struct as* as, vaddr_t vaddr, paddr_t paddr, size_t size,
               int flags)
{
    struct as_mapping *map;

    if (!size)
        return 0;

    /* Align size and addresses */
    vaddr = ((vaddr + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));
    paddr = ((paddr + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));
    size = ((size + PAGE_SIZE  - 1) & ~(PAGE_SIZE - 1));

    /* Locate a region */
    if (!vaddr)
    {
        vaddr = region_reserve(as, 0, size / PAGE_SIZE);

        if (!vaddr)
            goto error;
    }
    /* Try to map in kernel a user region */
    else if (vaddr >= KERNEL_BEGIN &&
             (as != &kernel_as || flags & AS_MAP_USER))
        goto error;

    if (!paddr)
    {
        paddr = segment_alloc(size / PAGE_SIZE);

        if (!paddr)
            goto segment_error;
    }
    /* Try to allocate user memory in kernel as */
    else if (vaddr < KERNEL_BEGIN && as == &kernel_as)
        goto error;

    map = kmalloc(sizeof (struct as_mapping));
    map->virt = vaddr;
    map->phy = paddr;
    map->size = size;

    if (!glue_call(as, map, as, vaddr, paddr, size, flags))
        goto arch_map_error;

    spinlock_lock(&as->map_lock);
    klist_add(&as->mapping, &map->list);
    spinlock_unlock(&as->map_lock);

    return vaddr;

arch_map_error:
    kfree(map);
segment_error:
    region_release(as, vaddr);
error:
    return 0;
}

void as_unmap(struct as *as, vaddr_t vaddr, int flags)
{
    struct as_mapping *map;

    spinlock_lock(&as->map_lock);

    klist_for_each_elem(&as->mapping, map, list)
    {
        if (map->virt == vaddr)
        {
            region_release(as, map->virt);

            if (flags & AS_UNMAP_RELEASE)
                segment_free(map->phy);

            glue_call(as, unmap, as, map->virt, map->size);

            klist_del(&map->list);

            kfree(map);

            spinlock_unlock(&as->map_lock);

            return;
        }
    }

    spinlock_unlock(&as->map_lock);
}

void as_destroy(struct as *as)
{
    klist_for_each(&as->mapping, mlist, list)
    {
        struct as_mapping *map = klist_elem(mlist, struct as_mapping, list);

        segment_free(map->phy);

        klist_del(&map->list);

        kfree(map);
    }

    klist_for_each(&as->regions, rlist, list)
    {
        struct region *reg = klist_elem(rlist, struct region, list);

        klist_del(&reg->list);

        kfree(reg);
    }

    glue_call(as, destroy, as);
}

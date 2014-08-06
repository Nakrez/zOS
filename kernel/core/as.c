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
    if (!__as.init(as))
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

int as_map(struct as* as, vaddr_t vaddr, paddr_t paddr, size_t size,
            int flags)
{
    struct as_mapping *map;

    /* Locate a region */
    if (!vaddr)
    {
        vaddr = region_reserve(as, 0, size / PAGE_SIZE);

        if (!vaddr)
            goto region_error;
    }

    if (!paddr)
    {
        paddr = segment_alloc(size / PAGE_SIZE);

        if (!paddr)
            goto segment_error;
    }

    map = kmalloc(sizeof (struct as_mapping));
    map->virt = vaddr;
    map->phy = paddr;
    map->size = size;

    if (!__as.map(vaddr, paddr, size, flags))
        goto arch_map_error;

    spinlock_lock(&as->map_lock);
    klist_add(&as->mapping, &map->list);
    spinlock_unlock(&as->map_lock);

arch_map_error:
    kfree(map);
segment_error:
    region_release(as, vaddr);
region_error:
    return 0;
}

void as_unmap(struct as *as, vaddr_t vaddr)
{
    struct as_mapping *map;

    spinlock_lock(&as->map_lock);

    klist_for_each_elem(&as->mapping, map, list)
    {
        if (map->virt == vaddr)
        {
            region_release(as, map->virt);

            segment_free(map->phy);

            __as.unmap(map->virt, map->size);

            klist_del(&map->list);

            kfree(map);

            spinlock_unlock(&as->map_lock);

            return;
        }
    }

    spinlock_unlock(&as->map_lock);
}

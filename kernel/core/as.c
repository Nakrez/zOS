#include <kernel/as.h>
#include <kernel/zos.h>
#include <kernel/kmalloc.h>
#include <kernel/region.h>
#include <kernel/segment.h>
#include <kernel/panic.h>

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
        map->phy = segment_locate(0);
        map->size = 1024 * PAGE_SIZE;

        klist_add(&as->mapping, &map->list);
    }

    return 1;
}

struct as_mapping *as_mapping_locate(struct as *as, vaddr_t vaddr)
{
    struct as_mapping *mapping;
    struct as_mapping *ret = NULL;

    spinlock_lock(&as->map_lock);

    klist_for_each_elem(&as->mapping, mapping, list)
    {
        if (mapping->virt <= vaddr && mapping->virt + mapping->size > vaddr)
        {
            ret = mapping;

            break;
        }
    }

    spinlock_unlock(&as->map_lock);

    return ret;
}

static struct as_mapping *setup_mapping(struct as *as, vaddr_t vaddr,
                                        paddr_t paddr, size_t size, int flags)
{
    struct as_mapping *map;

    /* We need to find a region */
    if (!vaddr)
    {
        map = kmalloc(sizeof (struct as_mapping));

        if (!map)
            goto error;

        map->size = 0;
        map->virt = region_reserve(as, 0, size / PAGE_SIZE);
        map->phy = 0;

        if (!map->virt)
            goto free_vaddr;
    }
    /* Trying to allocate kernel addresses for user */
    else if (vaddr >= KERNEL_BEGIN &&
             (as != &kernel_as || flags & AS_MAP_USER))
        goto error;
    else
    {
        /* Are we trying to override a previous mapping ? */

        /* No ? */
        if (!(map = as_mapping_locate(as, vaddr)))
        {
            if (!(map = kmalloc(sizeof (struct as_mapping))))
                goto error;

            map->size = 0;
            map->phy = 0;
        }
        /*
         * Yes ? Check size if the new area is smaller we need to split the
         * mapping otherwise it will be overwritten
         */
        else
        {
            spinlock_lock(&as->map_lock);

            /* We remove this mapping from mapping list */
            klist_del(&map->list);

            spinlock_unlock(&as->map_lock);

            if (map->size > size)
                kernel_panic("as: need mapping split");
        }
    }

    /* Remove previous physical pages if we override the area */
    if (map->phy)
        segment_release(map->phy);

    if (!paddr)
    {
        map->phy = segment_alloc(size / PAGE_SIZE);

        if (!map->phy)
            goto free_vaddr;
    }
    else
    {
        map->phy = segment_locate(paddr);

        if (!map->phy)
            goto free_vaddr;
    }

    /*
     * If a virtual address was given update mapping, otherwise this field
     * is already valid
     */
    if (vaddr)
        map->virt = vaddr;

    map->size = size;

    return map;

free_vaddr:
    /* Fresh allocation */
    if (!map->size)
    {
        /* An region was requested and found */
        if (!vaddr && map->virt)
            region_release(as, map->virt);

        kfree(map);
    }
error:
    return NULL;
}

vaddr_t as_map(struct as *as, vaddr_t vaddr, paddr_t paddr, size_t size,
               int flags)
{
    struct as_mapping *map;

    if (!size)
        return 0;

    /* Align size and addresses */
    vaddr = ((vaddr + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));
    paddr = ((paddr + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));
    size = ((size + PAGE_SIZE  - 1) & ~(PAGE_SIZE - 1));

    if (!(map = setup_mapping(as, vaddr, paddr, size, flags)))
        return 0;

    if (!glue_call(as, map, as, map->virt, map->phy->base, map->size, flags))
    {
        /* FIXME: more to do: release region and segment */
        kfree(map);

        return 0;
    }

    spinlock_lock(&as->map_lock);
    klist_add(&as->mapping, &map->list);
    spinlock_unlock(&as->map_lock);

    return map->virt;
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
                segment_release(map->phy);

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

        segment_release(map->phy);

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

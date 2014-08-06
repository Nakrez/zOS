#include <kernel/region.h>
#include <kernel/kmalloc.h>
#include <kernel/console.h>

#include <arch/mmu.h>

void region_initialize(struct as *as)
{
    struct region *reg = kmalloc(sizeof (struct region));

    /* Init region list */
    klist_head_init(&as->regions);

    if (as == &kernel_as)
    {
        reg->base = KERNEL_BEGIN;
        reg->page_size = 1024 + KERNEL_EXHEAP_SIZE / PAGE_SIZE;
        reg->mapped = 0;

        klist_add(&as->regions, &reg->list);

        /* Reserve kernel initial area */
        region_reserve(as, 0xC0000000, 1024);

        return;
    }

    reg->base = USER_BEGIN;
    reg->page_size = (KERNEL_BEGIN - USER_BEGIN) / PAGE_SIZE;
    reg->mapped = 0;

    klist_add(&as->regions, &reg->list);

    return;
}

static void region_split(struct region *reg, vaddr_t addr, size_t page_size)
{
    if (addr > reg->base)
    {
        struct region *before = kmalloc(sizeof (struct region));

        before->base = reg->base;
        before->page_size = (addr - reg->base) / PAGE_SIZE;
        before->mapped = 0;

        klist_add(reg->list.prev, &before->list);
    }

    if (addr + page_size * PAGE_SIZE < reg->base + reg->page_size * PAGE_SIZE)
    {
        struct region *after = kmalloc(sizeof (struct region));

        after->base = addr + page_size * PAGE_SIZE;
        after->page_size = reg->page_size -
                           (((addr - reg->base) / PAGE_SIZE) + page_size);
        after->mapped = 0;

        klist_add(&reg->list, &after->list);
    }

    reg->base = addr;
    reg->page_size = page_size;
}

vaddr_t region_reserve(struct as *as, vaddr_t addr, size_t page_size)
{
    struct region *reg;

    vaddr_t max_range = addr + page_size * PAGE_SIZE;

    spinlock_lock(&as->region_lock);

    klist_for_each_elem(&as->regions, reg, list)
    {
        if (reg->mapped)
            continue;

        vaddr_t regmax_range = reg->base + reg->page_size * PAGE_SIZE;

        if (!addr)
        {
            if (reg->page_size >= page_size)
            {
                reg->mapped = 1;

                region_split(reg, reg->base, page_size);

                spinlock_unlock(&as->region_lock);

                return reg->base;
            }
        }
        else if (addr >= reg->base && addr < regmax_range)
        {
            /* Address was found but area is too small */
            if (max_range >= regmax_range)
            {
                spinlock_unlock(&as->region_lock);
                return 0;
            }

            reg->mapped = 1;

            region_split(reg, addr, page_size);

            spinlock_unlock(&as->region_lock);

            return reg->base;
        }
    }

    spinlock_unlock(&as->region_lock);

    return 0;
}

static void region_merge(struct as *as, struct region *reg)
{
    if (reg->list.prev != &as->regions)
    {
        struct region *prev = klist_elem(reg->list.prev, struct region, list);

        if (!prev->mapped &&
            prev->base + reg->page_size * PAGE_SIZE == reg->base)
        {
            reg->base = prev->base;
            reg->page_size += prev->page_size;

            klist_del(&prev->list);

            kfree(prev);
        }
    }

    if (reg->list.next != &as->regions)
    {
        struct region *next = klist_elem(reg->list.next, struct region, list);

        if (!next->mapped &&
            reg->base + reg->page_size * PAGE_SIZE == next->base)
        {
            reg->page_size += next->page_size;

            klist_del(&next->list);

            kfree(next);
        }
    }
}

void region_release(struct as *as, vaddr_t addr)
{
    struct region *reg;

    spinlock_lock(&as->region_lock);

    klist_for_each_elem(&as->regions, reg, list)
    {
        if (reg->base == addr)
        {
            reg->mapped = 0;

            region_merge(as, reg);

            spinlock_unlock(&as->region_lock);

            return;
        }
    }

    spinlock_unlock(&as->region_lock);
}

void region_dump(struct as *as)
{
    struct region *reg;

    console_message(T_INF, "Region dump (page size : %u Ko)",
                    PAGE_SIZE / 1024);

    klist_for_each_elem(&as->regions, reg, list)
    {
        console_message(T_INF, "%s: 0x%x-0x%x (%u pages)",
                        reg->mapped ? "MAPPED" : "FREE",
                        reg->base, reg->base + reg->page_size * PAGE_SIZE,
                        reg->page_size);
    }
}

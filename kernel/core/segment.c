#include <kernel/zos.h>
#include <kernel/segment.h>
#include <kernel/panic.h>
#include <kernel/kmalloc.h>
#include <kernel/console.h>

#include <arch/spinlock.h>
#include <arch/mmu.h>

static spinlock_t segment_lock;

static struct klist segment_head;

void segment_initialize(struct boot_info *boot)
{
    struct segment *segment = NULL;

    glue_call(segment, init);

    klist_head_init(&segment_head);

    if (boot->segs_count == 0)
        kernel_panic("No memory map was provided by the bootloader");

    for (size_t i = 0; i < boot->segs_count; ++i)
    {
        segment = kmalloc(sizeof (struct segment));

        segment->base = boot->segs[i].seg_start;
        segment->page_size = boot->segs[i].seg_size / PAGE_SIZE;

        segment->free = 1;

        klist_add(&segment_head, &segment->list);
    }

    spinlock_init(&segment_lock);

    /* Reserve initial kernel memory */
    segment_reserve(0x0, 1024);

    segment_dump();
}

static void segment_split(struct segment *seg, paddr_t addr,
                          uint32_t page_size)
{
    if (addr > seg->base)
    {
        struct segment *before = kmalloc(sizeof (struct segment));

        before->free = 1;
        before->base = seg->base;
        before->page_size = (addr - seg->base) / PAGE_SIZE;

        klist_add(seg->list.prev, &before->list);
    }

    if (addr + page_size * PAGE_SIZE < seg->base + seg->page_size * PAGE_SIZE)
    {
        struct segment *after = kmalloc(sizeof (struct segment));

        after->free = 1;
        after->base = addr + page_size * PAGE_SIZE;
        after->page_size = seg->page_size -
                           (((addr - seg->base) / PAGE_SIZE) + page_size);

        klist_add(&seg->list, &after->list);
    }

    seg->page_size = page_size;
    seg->base = addr;
    seg->free = 0;
}

paddr_t segment_alloc(uint32_t page_size)
{
    struct segment *seg;

    if (!page_size)
        return 0;

    spinlock_lock(&segment_lock);

    klist_for_each_elem(&segment_head, seg, list)
    {
        if (!seg->free)
            continue;

        if (seg->page_size >= page_size)
        {
            segment_split(seg, seg->base, page_size);

            spinlock_unlock(&segment_lock);
            return seg->base;
        }
    }

    spinlock_unlock(&segment_lock);

    return 0;
}

int segment_reserve(paddr_t addr, uint32_t page_size)
{
    struct segment *seg;

    if (!page_size)
        return 0;

    spinlock_lock(&segment_lock);

    klist_for_each_elem(&segment_head, seg, list)
    {
        if (!seg->free)
            continue;

        uint32_t seg_size = seg->page_size * PAGE_SIZE;

        if (addr >= seg->base && addr <= seg->base + seg_size)
        {
            if (addr + page_size * PAGE_SIZE >
                seg->base + seg_size)
            {
                spinlock_unlock(&segment_lock);
                return 0;
            }

            segment_split(seg, addr, page_size);

            spinlock_unlock(&segment_lock);
            return 1;
        }
    }

    spinlock_unlock(&segment_lock);

    return 0;
}

static void segment_merge(struct segment *seg)
{
    if (seg->list.prev != &segment_head)
    {
        struct segment *prev;

        prev = klist_elem(seg->list.prev, struct segment, list);

        if (prev->free &&
            prev->base + prev->page_size * PAGE_SIZE == seg->base)
        {
            seg->base = prev->base;
            seg->page_size += prev->page_size;

            klist_del(&prev->list);

            kfree(prev);
        }
    }

    if (seg->list.next != &segment_head)
    {
        struct segment *next;

        next = klist_elem(seg->list.next, struct segment, list);

        if (next->free &&
            seg->base + seg->page_size * PAGE_SIZE == next->base)
        {
            seg->page_size += next->page_size;

            klist_del(&next->list);

            kfree(next);
        }
    }
}

void segment_free(paddr_t addr)
{
    struct segment *seg;

    spinlock_lock(&segment_lock);

    klist_for_each_elem(&segment_head, seg, list)
    {
        if (seg->base == addr)
        {
            if (seg->free)
                return;

            seg->free = 1;

            segment_merge(seg);

            spinlock_unlock(&segment_lock);

            return;
        }
    }

    spinlock_unlock(&segment_lock);
}

void segment_dump(void)
{
    struct segment *seg;

    console_message(T_INF, "Segment layout dump (page size : %u Ko)",
                    PAGE_SIZE / 1024);

    spinlock_lock(&segment_lock);

    klist_for_each_elem(&segment_head, seg, list)
    {
        console_message(T_INF, "%s: 0x%x-0x%x (%u pages)",
                        seg->free ? "FREE" : "USED",
                        seg->base, seg->base + seg->page_size * PAGE_SIZE,
                        seg->page_size);
    }

    spinlock_unlock(&segment_lock);
}

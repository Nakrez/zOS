#include <boot/boot.h>

#include <kernel/segment.h>
#include <kernel/panic.h>
#include <kernel/kmalloc.h>
#include <kernel/console.h>

#include <arch/mmu.h>

struct klist segment_head;

struct segment *segments;

void segment_initialize(struct boot_info *boot)
{
    struct segment *segment = NULL;

    __segment.init();

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

    segment_dump();
}

void segment_dump(void)
{
    struct segment *seg;

    console_message(T_INF, "segment layout dump (page size : %uo)", PAGE_SIZE);

    klist_for_each_elem(&segment_head, seg, list)
    {
        console_message(T_INF, "%s: 0x%x-0x%x (%L pages)",
                        seg->free ? "FREE" : "USED",
                        seg->base, seg->base + seg->page_size * PAGE_SIZE,
                        seg->page_size);
    }
}

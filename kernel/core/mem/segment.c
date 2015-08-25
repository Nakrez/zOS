/*
 * zOS
 * Copyright (C) 2014 - 2015 Baptiste Covolato
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with zOS.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file    kernel/core/mem/segment.c
 * \brief   Generic implementation of physical memory management
 *
 * \author  Baptiste Covolato
 */

#include <kernel/zos.h>
#include <kernel/panic.h>
#include <kernel/console.h>
#include <kernel/errno.h>

#include <kernel/mem/segment.h>
#include <kernel/mem/kmalloc.h>

#include <arch/spinlock.h>
#include <arch/mmu.h>

static spinlock_t segment_lock;

static struct klist segment_head;

void segment_initialize(struct boot_info *boot)
{
    int ret;

    glue_call(segment, init);

    klist_head_init(&segment_head);

    if (boot->segs_count == 0)
        kernel_panic("No memory map was provided by the bootloader");

    for (size_t i = 0; i < boot->segs_count; ++i) {
        ret = segment_add(boot->segs[i].seg_start,
                          boot->segs[i].seg_size / PAGE_SIZE);
        if (ret < 0)
            kernel_panic("Fail to add a segment");
    }

    spinlock_init(&segment_lock);

    /* Reserve initial kernel memory */
    segment_reserve(0x0, 160);
    segment_reserve(0xA0000, 64);
    segment_reserve(0xE0000, 800);

    segment_dump();
}

int segment_add(uintptr_t base, uint32_t page_size)
{
    struct segment *seg;

    seg = kmalloc(sizeof (struct segment));
    if (!seg)
        return -ENOMEM;

    seg->base = base;
    seg->page_size = page_size;

    seg->ref_count = 0;
    seg->flags = SEGMENT_FLAGS_NONE;

    spinlock_lock(&segment_lock);

    klist_add(&segment_head, &seg->list);

    spinlock_unlock(&segment_lock);

    return 0;
}

static void segment_split(struct segment *seg, paddr_t addr,
                          uint32_t page_size)
{
    if (addr > seg->base)
    {
        struct segment *before = kmalloc(sizeof (struct segment));

        before->ref_count = 0;
        before->flags = SEGMENT_FLAGS_NONE;
        before->base = seg->base;
        before->page_size = (addr - seg->base) / PAGE_SIZE;

        klist_add(seg->list.prev, &before->list);
    }

    if (addr + page_size * PAGE_SIZE < seg->base + seg->page_size * PAGE_SIZE)
    {
        struct segment *after = kmalloc(sizeof (struct segment));

        after->ref_count = 0;
        after->flags = SEGMENT_FLAGS_NONE;
        after->base = addr + page_size * PAGE_SIZE;
        after->page_size = seg->page_size -
                           (((addr - seg->base) / PAGE_SIZE) + page_size);

        klist_add(&seg->list, &after->list);
    }

    seg->page_size = page_size;
    seg->base = addr;
    seg->ref_count = 1;
}

struct segment *segment_alloc(uint32_t page_size)
{
    struct segment *seg;

    if (!page_size)
        return NULL;

    spinlock_lock(&segment_lock);

    klist_for_each_elem(&segment_head, seg, list)
    {
        if (seg->ref_count)
            continue;

        if (seg->page_size >= page_size)
        {
            segment_split(seg, seg->base, page_size);

            spinlock_unlock(&segment_lock);
            return seg;
        }
    }

    spinlock_unlock(&segment_lock);

    return NULL;
}

int segment_reserve(paddr_t addr, uint32_t page_size)
{
    struct segment *seg;

    if (!page_size)
        return 0;

    spinlock_lock(&segment_lock);

    klist_for_each_elem(&segment_head, seg, list)
    {
        if (seg->ref_count)
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

paddr_t segment_alloc_address(uint32_t page_size)
{
    struct segment *seg;

    seg = segment_alloc(page_size);

    if (!seg)
        return 0;

    return seg->base;
}

struct segment *segment_locate(paddr_t addr)
{
    struct segment *seg;

    spinlock_lock(&segment_lock);

    klist_for_each_elem(&segment_head, seg, list)
    {
        if (seg->base <= addr
            && seg->base + seg->page_size * PAGE_SIZE > addr)
        {
            spinlock_unlock(&segment_lock);

            return seg;
        }
    }

    spinlock_unlock(&segment_lock);

    return NULL;
}

static void segment_merge(struct segment *seg)
{
    if (seg->list.prev != &segment_head)
    {
        struct segment *prev;

        prev = klist_elem(seg->list.prev, struct segment, list);

        if (!prev->ref_count &&
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

        if (!next->ref_count &&
            seg->base + seg->page_size * PAGE_SIZE == next->base)
        {
            seg->page_size += next->page_size;

            klist_del(&next->list);

            kfree(next);
        }
    }
}

void segment_release(struct segment *seg)
{
    if (!seg || !seg->ref_count)
        return;

    spinlock_lock(&segment_lock);

    --seg->ref_count;

    if (!seg->ref_count)
        segment_merge(seg);

    spinlock_unlock(&segment_lock);
}

void segment_free(paddr_t addr)
{
    struct segment *seg;

    spinlock_lock(&segment_lock);

    klist_for_each_elem(&segment_head, seg, list)
    {
        if (seg->base == addr)
        {
            if (!seg->ref_count)
                return;

            --seg->ref_count;

            if (!seg->ref_count)
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
        console_message(T_INF, "%u time referenced: 0x%x-0x%x (%u pages)",
                        seg->ref_count,
                        seg->base, seg->base + seg->page_size * PAGE_SIZE,
                        seg->page_size);
    }

    spinlock_unlock(&segment_lock);
}

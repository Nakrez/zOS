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
 * \file    kernel/core/mem/kmalloc.c
 * \brief   Implementation of kernel memory management functions
 *
 * \author  Baptiste Covolato
 */

#include <string.h>

#include <kernel/zos.h>
#include <kernel/panic.h>
#include <kernel/console.h>
#include <kernel/klist.h>

#include <kernel/mem/kmalloc.h>

#include <arch/spinlock.h>

struct kmalloc_blk
{
    void *ptr;
    uint32_t size;
    uint8_t free;

    struct klist list;
};

static spinlock_t kmalloc_lock;

static struct klist kmalloc_head;

void kmalloc_initialize(struct boot_info *boot)
{
    static struct kmalloc_blk *first_blk;

    spinlock_init(&kmalloc_lock);
    klist_head_init(&kmalloc_head);

    first_blk = (void *)boot->heap_start;

    first_blk->ptr = first_blk + 1;
    first_blk->size = boot->heap_size - KSTACK_SIZE -
                      sizeof (struct kmalloc_blk);
    first_blk->free = 1;

    klist_add(&kmalloc_head, &first_blk->list);

    console_message(T_OK, "kmalloc initialisation");
    console_message(T_INF, "kmalloc initial heap: 0x%x-0x%x (%u Ko)",
                    first_blk->ptr, first_blk->ptr + first_blk->size,
                    first_blk->size / 1024);
}

static void *kmalloc_split(struct kmalloc_blk *blk, size_t size)
{
    struct kmalloc_blk *new_blk;

    blk->free = 0;

    /* Split needed */
    if (blk->size > size + sizeof (struct kmalloc_blk))
    {
        new_blk = ((void *)(blk + 1)) + size;
        new_blk->size = blk->size - size - sizeof (struct kmalloc_blk);
        new_blk->ptr = new_blk + 1;
        new_blk->free = 1;

        blk->size = size;

        klist_add(&blk->list, &new_blk->list);
    }

    return blk->ptr;
}

void *kmalloc(size_t size)
{
    struct kmalloc_blk *blk;

    spinlock_lock(&kmalloc_lock);

    klist_for_each_elem(&kmalloc_head, blk, list)
    {
        if (blk->size >= size && blk->free)
        {
            void *addr = kmalloc_split(blk, size);

            spinlock_unlock(&kmalloc_lock);

            return addr;
        }
    }

    spinlock_unlock(&kmalloc_lock);

    kernel_panic("kmalloc no more memory");

    return NULL;
}

static void kmerge(struct kmalloc_blk *blk)
{
    if (blk->list.next != &kmalloc_head)
    {
        struct kmalloc_blk *next = klist_elem(blk->list.next,
                                              struct kmalloc_blk, list);

        if (next->free && next == ((void *)(blk + 1)) + blk->size)
        {
            blk->size += next->size + sizeof (struct kmalloc_blk);

            klist_del(&next->list);
        }
    }

    if (blk->list.prev != &kmalloc_head)
    {
        struct kmalloc_blk *prev = klist_elem(blk->list.prev,
                                              struct kmalloc_blk, list);

        if (prev->free && blk == ((void *)(prev + 1)) + prev->size)
        {
            prev->size += blk->size + sizeof (struct kmalloc_blk);

            klist_del(&blk->list);
        }
    }
}

void kfree(void *ptr)
{
    struct kmalloc_blk *blk;

    if (!ptr)
        return;

    blk = ptr;
    --blk;

    if (blk->ptr != ptr)
        kernel_panic("kfree junk ptr");

    spinlock_lock(&kmalloc_lock);

    blk->free = 1;

    kmerge(blk);

    spinlock_unlock(&kmalloc_lock);
}

void *krealloc(void *ptr, size_t new_size)
{
    void *new_area;
    struct kmalloc_blk *old_blk;

    if (!ptr)
        return kmalloc(new_size);

    old_blk = ptr;
    --old_blk;

    if (old_blk->ptr != ptr)
        kernel_panic("krealloc junk ptr");

    if (!(new_area = kmalloc(new_size)))
        return NULL;

    memcpy(new_area, ptr, new_size > old_blk->size ? old_blk->size : new_size);

    kfree(ptr);

    return new_area;
}

void kmalloc_dump(void)
{
    struct kmalloc_blk *blk;

    console_message(T_INF, "kmalloc dump");

    spinlock_lock(&kmalloc_lock);

    klist_for_each_elem(&kmalloc_head, blk, list)
    {
        console_message(T_INF, "%s: 0x%x-0x%x (%u o)",
                        blk->free ? "FREE" : "USED",
                        blk->ptr, blk->ptr + blk->size, blk->size);
    }

    spinlock_unlock(&kmalloc_lock);
}

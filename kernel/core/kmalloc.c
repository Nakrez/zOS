#include <kernel/kmalloc.h>
#include <kernel/zos.h>
#include <kernel/console.h>
#include <kernel/klist.h>

struct kmalloc_blk
{
    void *blk;
    uint32_t size;
    uint8_t free;

    struct klist list;
};

struct kmalloc_blk *first_blk;

void kmalloc_initialize(struct boot_info *boot)
{
    first_blk = (void *)boot->heap_start;

    first_blk->blk = first_blk + 1;
    first_blk->size = boot->heap_size - KSTACK_SIZE -
                      sizeof (struct kmalloc_blk);
    first_blk->free = 1;
    first_blk->list.next = NULL;
    first_blk->list.prev = NULL;

    console_message(T_OK, "kmalloc initialisation");
    console_message(T_INF, "kmalloc initial heap: 0x%x-0x%x (%u Ko)",
                    first_blk->blk, first_blk->blk + first_blk->size,
                    first_blk->size / 1024);
}

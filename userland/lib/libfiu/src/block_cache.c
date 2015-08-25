#include <stdlib.h>

#include <fiu/fiu.h>

int fiu_cache_initialize(struct fiu_instance *fi, size_t cache_size,
                         size_t block_size, cache_fetch_t fetch,
                         cache_flush_t flush)
{
    struct fiu_block *block;

    if (!fetch)
        /* XXX: -EINVAL */
        return -1;

    fi->block_cache = malloc(sizeof (struct fiu_cache));
    if (!fi->block_cache)
        /* XXX: -ENOMEM */
        return -1;

    fi->block_cache->blocks_head = malloc(sizeof (struct fiu_block) *
                                          cache_size + block_size *
                                          cache_size);
    if (!fi->block_cache->blocks_head) {
        free(fi->block_cache);
        /* XXX: -ENOMEM */
        return -1;
    }

    fi->block_cache->cache_size = cache_size;
    fi->block_cache->block_size = block_size;
    fi->block_cache->fetch = fetch;
    fi->block_cache->flush = flush;

    fi->block_cache->blocks_tail = NULL;

    spinlock_init(&fi->block_cache->cache_lock);

    block = fi->block_cache->blocks_head;

    for (size_t i = 0; i < cache_size; ++i) {
        block->block_num = (uint32_t)-1;
        block->ref_count = 0;
        block->block = block + 1;
        block->next = (void *)(((char *)(block + 1)) + block_size);
        block->prev = fi->block_cache->blocks_tail;

        fi->block_cache->blocks_tail = block;
        block = block->next;
    }

    fi->block_cache->blocks_tail->next = NULL;

    return 0;
}

void *fiu_cache_request(struct fiu_instance *fi, uint32_t block)
{
    struct fiu_block *free = NULL;
    struct fiu_block *fblock;

    spinlock_lock(&fi->block_cache->cache_lock);

    fblock = fi->block_cache->blocks_head;

    for (size_t i = 0; i < fi->block_cache->cache_size; ++i) {
        if (fblock->block_num == block) {
            ++fblock->ref_count;
            spinlock_unlock(&fi->block_cache->cache_lock);
            return fblock->block;
        }

        if (!free && fblock->ref_count <= 0)
            free = fblock;

        fblock = fblock->next;
    }

    if (!free) {
        spinlock_unlock(&fi->block_cache->cache_lock);
        return NULL;
    }

    if (fi->block_cache->fetch(fi, free->block, block) < 0) {
        spinlock_unlock(&fi->block_cache->cache_lock);
        return NULL;
    }

    free->block_num = block;
    free->ref_count = 1;

    spinlock_unlock(&fi->block_cache->cache_lock);

    return free->block;
}

static void fiu_cache_put_back(struct fiu_instance *fi, struct fiu_block *b)
{
    /* Already at the end of the list */
    if (!b->next)
        return;

    /* Head of the list */
    if (!b->prev) {
        fi->block_cache->blocks_head = b->next;
        b->next->prev = NULL;
    } else {
        b->prev->next = b->next;
        b->next->prev = b->prev;
    }

    b->prev = fi->block_cache->blocks_tail;
    fi->block_cache->blocks_tail->next = b;
    b->next = NULL;

    fi->block_cache->blocks_tail = b;
}

void fiu_cache_release(struct fiu_instance *fi, uint32_t block)
{
    struct fiu_block *fblock;

    spinlock_lock(&fi->block_cache->cache_lock);

    fblock = fi->block_cache->blocks_head;

    while (fblock) {
        if (fblock->block_num == block) {
            --fblock->ref_count;

            if (fblock->ref_count == 0)
                fiu_cache_put_back(fi, fblock);

            spinlock_unlock(&fi->block_cache->cache_lock);

            return;
        }

        fblock = fblock->next;
    }

    spinlock_unlock(&fi->block_cache->cache_lock);
}

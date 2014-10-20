#include <stdlib.h>

#include <fiu/fiu.h>

int fiu_cache_initialize(struct fiu_internal *fiu, size_t cache_size,
                         size_t block_size, cache_fetch_t fetch,
                         cache_flush_t flush)
{
    struct fiu_block *block;

    if (!fetch)
        return -1;

    if (!(fiu->block_cache = malloc(sizeof (struct fiu_cache))))
        return -1;

    block = malloc(sizeof (struct fiu_block) * cache_size + block_size *
                   cache_size);

    fiu->block_cache->blocks_head = block;

    if (!fiu->block_cache->blocks_head)
    {
        free(fiu->block_cache);

        return -1;
    }

    fiu->block_cache->cache_size = cache_size;
    fiu->block_cache->block_size = block_size;
    fiu->block_cache->fetch = fetch;
    fiu->block_cache->flush = flush;

    spinlock_init(&fiu->block_cache->cache_lock);

    block = fiu->block_cache->blocks_head;

    for (size_t i = 0; i < cache_size; ++i)
    {
        block->block_num = (uint32_t)-1;
        block->ref_count = 0;
        block->block = block + 1;
        block->next = (void *)(((char *)(block + 1)) + block_size);

        fiu->block_cache->blocks_tail = block;
        block = block->next;
    }

    fiu->block_cache->blocks_tail->next = NULL;

    return 0;
}

void *fiu_cache_request(struct fiu_internal *fiu, uint32_t block)
{
    struct fiu_block *free = NULL;
    struct fiu_block *fblock;

    spinlock_lock(&fiu->block_cache->cache_lock);

    fblock = fiu->block_cache->blocks_head;

    for (size_t i = 0; i < fiu->block_cache->cache_size; ++i)
    {
        if (fblock->block_num == block)
        {
            ++fblock->ref_count;

            spinlock_unlock(&fiu->block_cache->cache_lock);

            return fblock->block;
        }

        if (!free && fblock->ref_count <= 0)
            free = fblock;

        fblock = fblock->next;
    }

    if (!free)
    {
        spinlock_unlock(&fiu->block_cache->cache_lock);

        return NULL;
    }

    if (fiu->block_cache->fetch(fiu, free->block, block) < 0)
    {
        spinlock_unlock(&fiu->block_cache->cache_lock);

        return NULL;
    }

    free->block_num = block;
    free->ref_count = 1;

    spinlock_unlock(&fiu->block_cache->cache_lock);

    return free->block;
}

void fiu_cache_release(struct fiu_internal *fiu, uint32_t block)
{
    struct fiu_block *fblock;

    spinlock_lock(&fiu->block_cache->cache_lock);

    fblock = fiu->block_cache->blocks_head;

    for (size_t i = 0; i < fiu->block_cache->cache_size; ++i)
    {
        if (fblock->block_num == block)
        {
            /* TODO: Add at the end of the list */
            --fblock->ref_count;

            spinlock_unlock(&fiu->block_cache->cache_lock);

            return;
        }
    }

    spinlock_unlock(&fiu->block_cache->cache_lock);
}

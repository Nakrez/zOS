#ifndef FIU_BLOCK_CACHE_H
# define FIU_BLOCK_CACHE_H

# include <stdint.h>

# include <sys/spinlock.h>

struct fiu_internal;

typedef int (*cache_fetch_t)(struct fiu_internal *, void *, uint32_t);
typedef int (*cache_flush_t)(struct fiu_internal *, void *, uint32_t);

struct fiu_block {
    uint32_t block_num;

    int ref_count;

    void *block;

    struct fiu_block *next;
};

struct fiu_cache {
    size_t cache_size;
    size_t block_size;

    struct fiu_block *blocks_head;
    struct fiu_block *blocks_tail;

    spinlock_t cache_lock;

    cache_fetch_t fetch;
    cache_flush_t flush;
};

/* Initialize block cache
 *
 * fiu: The fiu structure
 * cache_size: The size of cache (in number of block)
 * block_size: The size of a block
 * fetch: The function to fetch a block (Must be != NULL)
 * flush: The function to flush a block (Can be NULL)
 *
 * return: 0 if sucess, < 0 if fail
 */
int fiu_cache_initialize(struct fiu_internal *fiu, size_t cache_size,
                         size_t block_size, cache_fetch_t fetch,
                         cache_flush_t flush);

/* Request a block from cache
 *
 * fiu: The fiu structure
 * block: The number of the block
 *
 * return: NULL if fail, the block otherwise
 */
void *fiu_cache_request(struct fiu_internal *fiu, uint32_t block);

/* Release a block from cache
 *
 * fiu: The fiu structure
 * block: The number of the block
 */
void fiu_cache_release(struct fiu_internal *fiu, uint32_t block);

#endif /* !FIU_BLOCK_CACHE_H */

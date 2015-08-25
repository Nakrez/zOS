#ifndef FIU_BLOCK_CACHE_H
# define FIU_BLOCK_CACHE_H

# include <stdint.h>

# include <sys/spinlock.h>

struct fiu_instance;

typedef int (*cache_fetch_t)(struct fiu_instance *, void *, uint32_t);
typedef int (*cache_flush_t)(struct fiu_instance *, void *, uint32_t);

struct fiu_block {
    uint32_t block_num;

    int ref_count;

    void *block;

    struct fiu_block *next;
    struct fiu_block *prev;
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

/**
 *  \brief  Initialize block cache
 *
 *  \param  fi          The file system instance
 *  \param  cache_size  The size of cache (in number of block)
 *  \param  block_size  The size of a block
 *  \param  fetch       The function to fetch a block (Must be != NULL)
 *  \param  flush       The function to flush a block (Can be NULL)
 *
 *  \return 0 if sucess, < 0 if fail
 */
int fiu_cache_initialize(struct fiu_instance *fi, size_t cache_size,
                         size_t block_size, cache_fetch_t fetch,
                         cache_flush_t flush);

/**
 *  \brief  Request a block from cache
 *
 *  \param  fi      The file system instance
 *  \param  block   The number of the block
 *
 *  \return NULL if fail, the block otherwise
 */
void *fiu_cache_request(struct fiu_instance *fi, uint32_t block);

/**
 *  \brief  Release a block from cache
 *
 *  \param  fi      The file system instance
 *  \parem  block   The number of the block
 */
void fiu_cache_release(struct fiu_instance *fi, uint32_t block);

#endif /* !FIU_BLOCK_CACHE_H */

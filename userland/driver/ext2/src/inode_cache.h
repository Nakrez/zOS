#ifndef EXT2_INODE_CACHE_H
# define EXT2_INODE_CACHE_H

# include <stdint.h>

# include "fs.h"

# define EXT2_INODE_CACHE_SIZE 32

# define EXT2_BAD_INODE 0

struct ext2_cinode {
    ino_t inode;

    int ref_count;

    struct ext2_inode cinode;
};

/* Initialize ext2 inode cache
 *
 * ext2: The ext2 filesystem structure
 *
 * return: 1 on sucess, 0 otherwise
 */

int ext2_icache_initialize(struct ext2fs *ext2);

/* Request an inode from the cache (or fetched if not present)
 *
 * ext2: The ext2 filesystem structure
 * inode: The number of the inode you want to fetch
 *
 * return: The inode if success, NULL otherwise
 */

struct ext2_inode *ext2_icache_request(struct ext2fs *ext2, ino_t inode);

/* Release an inode from the cache (decrement reference counter)
 *
 * ext2: The ext2 filesystem structure
 * inode: The inode you want to free
 */

void ext2_icache_release(struct ext2fs *ext2, ino_t inode);

#endif /* !EXT2_INODE_CACHE_H */

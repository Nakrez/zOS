#include <unistd.h>
#include <stdlib.h>

#include <zos/print.h>

#include "inode_cache.h"

int ext2_icache_initialize(struct ext2fs *ext2)
{
    ext2->inode_cache = malloc(sizeof (struct ext2_cinode) *
                               EXT2_INODE_CACHE_SIZE);

    if (!ext2->inode_cache)
        /* XXX: ENOMEM */
        return -1;

    for (int i = 0; i < EXT2_INODE_CACHE_SIZE; ++i) {
        ext2->inode_cache[i].inode = EXT2_BAD_INODE;
        ext2->inode_cache[i].ref_count = 0;
    }

    /* Cache root inode */
    if (ext2_icache_request(ext2, 2) == NULL)
        return -1;

    return 0;
}

struct ext2_inode *ext2_icache_request(struct ext2fs *ext2, ino_t inode)
{
    int i;
    uint64_t group;
    uint64_t index;
    int ret;
    struct fiu_instance *fi = ext2->fi;

    for (i = 0; i < EXT2_INODE_CACHE_SIZE; ++i) {
        if (ext2->inode_cache[i].inode == inode) {
            ++ext2->inode_cache[i].ref_count;
            return &(ext2->inode_cache[i].cinode);
        }
    }

    /*
     * If the inode was not in cache, we find a place to hold it in the cache
     * and we fetch it
     */

    for (i = 0; i < EXT2_INODE_CACHE_SIZE; ++i) {
        if (ext2->inode_cache[i].inode == EXT2_BAD_INODE ||
            ext2->inode_cache[i].ref_count == 0)
            break;
    }

    /* No place to hold it */
    if (i == EXT2_INODE_CACHE_SIZE) {
        uprint("EXT2: Cache is full! FIXME!");
        return NULL;
    }

    index = (inode - 1) % ext2->sb.inode_per_group;
    group = (inode - 1) / ext2->sb.inode_per_group;

    off_t lseek_off = (off_t)ext2->grp_table[group].inode_table *
                      ext2->block_size + index * ext2->sb.sizeof_inode;

    ret = lseek(fi->device_fd, lseek_off, SEEK_SET);
    if (ret < 0)
        return NULL;

    ret = read(fi->device_fd, &ext2->inode_cache[i].cinode,
               sizeof (struct ext2_inode));
    if (ret < 0)
        return NULL;

    ext2->inode_cache[i].inode = inode;
    ext2->inode_cache[i].ref_count = 1;

    return &ext2->inode_cache[i].cinode;
}

void ext2_icache_release(struct ext2fs *ext2, ino_t inode)
{
    for (int i = 0; i < EXT2_INODE_CACHE_SIZE; ++i) {
        if (ext2->inode_cache[i].inode == inode) {
            --ext2->inode_cache[i].ref_count;
            return;
        }
    }
}

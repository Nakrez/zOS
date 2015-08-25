#include "inode.h"
#include "fs.h"

int inode_block_data(struct ext2fs *ext2, struct ext2_inode *inode,
                     uint64_t offset, uint32_t *block)
{
    uint64_t size = inode->lower_size;

    if ((ext2->sb.minor > 0 || ext2->sb.major > 0) &&
        (!(inode->type_perm & EXT2_TYPE_DIRECTORY)))
            size |= ((uint64_t)inode->directory_acl) << 32;

    if (offset > size)
        return -1;

    if (offset < 12 * ext2->block_size) {
        *block = inode->dbp[offset / ext2->block_size];
        return 0;
    }

    offset -= 12 * ext2->block_size;

    /* Singly indirect block ptr */
    if (offset < (ext2->block_size / sizeof (uint32_t)) * ext2->block_size) {
        uint32_t *blk = fiu_cache_request(ext2->fi, inode->singly_ibp);

        if (!blk)
            return -1;

        *block = blk[offset / ext2->block_size];

        fiu_cache_release(ext2->fi, inode->singly_ibp);

        return 0;
    }

    return -1;
}

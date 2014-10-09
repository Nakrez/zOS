#include <unistd.h>

#include "fs.h"
#include "block.h"

int ext2_block_fetch(struct fiu_internal *fiu, void *buffer, uint32_t block)
{
    int res;
    struct ext2fs *ext2 = fiu->private;

    if ((res = lseek(ext2->fd, block * ext2->block_size, SEEK_SET)) < 0)
        return res;

    if ((res = read(ext2->fd, buffer, ext2->block_size)) < 0)
        return res;

    if ((size_t)res != ext2->block_size)
        return -1;

    return 0;
}

int ext2_block_flush(struct fiu_internal *fiu, void *buffer, uint32_t block)
{
    (void) fiu;
    (void) buffer;
    (void) block;

    return 0;
}

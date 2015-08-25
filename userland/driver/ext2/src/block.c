#include <unistd.h>

#include <fiu/fiu.h>

#include "fs.h"
#include "block.h"

int ext2_block_fetch(struct fiu_instance *fi, void *buffer, uint32_t block)
{
    int ret;
    struct ext2fs *ext2 = fi->private;
    off_t block_off = block * (off_t)ext2->block_size;

    ret = lseek(fi->device_fd, block_off, SEEK_SET);
    if (ret < 0)
        return ret;

    ret = read(fi->device_fd, buffer, ext2->block_size);
    if (ret < 0)
        return ret;

    if ((size_t)ret != ext2->block_size)
        return -1;

    return 0;
}

int ext2_block_flush(struct fiu_instance *fi, void *buffer, uint32_t block)
{
    (void) fi;
    (void) buffer;
    (void) block;

    return 0;
}

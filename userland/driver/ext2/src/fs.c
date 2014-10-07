#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include "fs.h"

# define EXT2FS_OPEN_TIMEOUT 1000
# define EXT2FS_OPEN_RETRY 50

static int ext2fs_load_group_table(struct ext2fs *ext2)
{
    int ret;
    size_t group_nb;
    size_t inode_nb;
    size_t gt_size;
    size_t offset;

    group_nb = (ext2->sb.total_blocks / ext2->sb.block_per_group) +
               ((ext2->sb.total_blocks % ext2->sb.block_per_group) ? 1 : 0);

    inode_nb = (ext2->sb.total_inodes / ext2->sb.inode_per_group) +
               ((ext2->sb.total_inodes % ext2->sb.inode_per_group) ? 1 : 0);

    gt_size = (group_nb > inode_nb ? group_nb : inode_nb);

    ext2->grp_table = malloc(sizeof (struct ext2_group_descriptor) * gt_size);

    if (!ext2->grp_table)
        return 0;

    offset = ext2->sb.block_size == 1024 ? 2048 : ext2->sb.block_size;

    lseek(ext2->fd, offset, SEEK_SET);

    ret = read(ext2->fd, ext2->grp_table,
               sizeof (struct ext2_group_descriptor) * gt_size);

    if (ret < 0)
        return 0;

    return (size_t)ret == sizeof (struct ext2_group_descriptor) * gt_size;
}

int ext2fs_initialize(struct ext2fs *ext2, const char *disk)
{
    int ret;
    int read_size = 0;
    int timeout = 0;

    /* Wait for the disk driver to be alive */
    while (timeout < EXT2FS_OPEN_TIMEOUT)
    {
        ext2->fd = open(disk, 0, 0);

        if (ext2->fd >= 0)
            break;

        timeout += EXT2FS_OPEN_RETRY;
        usleep(EXT2FS_OPEN_RETRY);
    }

    if (ext2->fd < 0)
        return 0;

    /* Superblock is always 1024 bytes after the beginning */
    lseek(ext2->fd, 1024, SEEK_CUR);

    while (read_size != sizeof (struct ext2_superblock))
    {
        ret = read(ext2->fd, ((char *)&ext2->sb) + read_size,
                   sizeof (struct ext2_superblock) - read_size);

        if (ret < 0)
            break;

        read_size += ret;
    }

    if (ret < 0)
        return 0;

    if (ext2->sb.magic != EXT2_SB_MAGIC)
        return 0;

    return ext2fs_load_group_table(ext2);
}

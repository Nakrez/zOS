#include <unistd.h>
#include <fcntl.h>

#include "fs.h"

# define EXT2FS_OPEN_TIMEOUT 1000
# define EXT2FS_OPEN_RETRY 50

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

    return 1;
}

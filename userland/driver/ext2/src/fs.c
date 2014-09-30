#include <unistd.h>
#include <fcntl.h>

#include "fs.h"

# define EXT2FS_OPEN_TIMEOUT 1000
# define EXT2FS_OPEN_RETRY 50

static struct ext2fs fs = {
    .fd = -1,
};

int ext2fs_initialize(const char *disk)
{
    int ret;
    int read_size = 0;
    int timeout = 0;

    /* Wait for the disk driver to be alive */
    while (timeout < EXT2FS_OPEN_TIMEOUT)
    {
        fs.fd = open(disk, 0, 0);

        if (fs.fd >= 0)
            break;

        timeout += EXT2FS_OPEN_RETRY;
        usleep(EXT2FS_OPEN_RETRY);
    }

    if (fs.fd < 0)
        return 0;

    /* Superblock is always 1024 bytes after the beginning */
    lseek(fs.fd, 1024, SEEK_CUR);

    while (read_size != sizeof (struct ext2_superblock))
    {
        ret = read(fs.fd, ((char *)&fs.sb) + read_size,
                   sizeof (struct ext2_superblock) - read_size);

        if (ret < 0)
            break;

        read_size += ret;
    }

    if (ret < 0)
        return 0;

    if (fs.sb.magic != EXT2_SB_MAGIC)
        return 0;

    return 1;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <zos/print.h>

#include "fs.h"
#include "file.h"

static struct fiu_ops ext2_ops = {
    .root_remount = ext2_root_remount,
    .lookup = ext2fs_lookup,
    .stat = ext2fs_stat,
    .open = ext2fs_open,
    .read = ext2fs_read,
    .getdirent = ext2fs_getdirent,
    .close = ext2fs_close,
};

void usage()
{
    uprint("EXT2 DEVICE DIR");
}

int main(int argc, char *argv[])
{
    int ret;
    struct ext2fs *ext2;

    if (argc < 3)
        usage();

    if (!(ext2 = malloc(sizeof (struct ext2fs))))
    {
        uprint("EXT2: Cannot allocate memory");

        return 1;
    }

    ext2->fiu.private = ext2;

    if (!ext2fs_initialize(ext2, argv[1]))
    {
        uprint("EXT2: an error occured in initialization. Bye!");

        return 1;
    }

    /* FIXME: dynamically generate device name */
    ret = fiu_create("ext2-ata-disk0", 0755, &ext2_ops, &ext2->fiu);

    if (ret < 0)
    {
        uprint("EXT2: fiu_create() failed");

        return 1;
    }

    /* 15 = "EXT2: Mounting ", 4 = " on " */
    char *buf = malloc(15 + strlen(argv[1]) + 4 + strlen(argv[2]) + 1);

    if (!buf)
    {
        uprint("EXT2: Out of memory");

        return 1;
    }

    sprintf(buf, "EXT2: Mounting %s on %s", argv[1], argv[2]);

    uprint(buf);

    free(buf);

    ret = fiu_main(&ext2->fiu, argv[2]);

    if (ret < 0)
    {
        uprint("EXT2: fiu_main() failed");

        return 1;
    }

    return ret;
}

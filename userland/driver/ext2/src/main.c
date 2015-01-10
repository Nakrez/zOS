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
    uprint("EXT2 [OPTS] DEVICE DIR");
    uprint("--daemon: Launch ext2 as a daemon");
}

int main(int argc, char *argv[])
{
    int ret;
    struct ext2fs *ext2;
    char *mount_pt;
    char *device;
    char *device_name;
    char *ext2_device;

    if (argc < 3)
    {
        usage();

        return 1;
    }

    device = argv[1];
    mount_pt = argv[2];

    char *tmp = strrchr(device, '/');

    if (!tmp || !*(tmp + 1))
        device_name = device;
    else
        device_name = tmp + 1;

    if (!(ext2 = malloc(sizeof (struct ext2fs))))
    {
        uprint("EXT2: Cannot allocate memory");

        return 1;
    }

    ext2->fiu.private = ext2;

    if (!ext2fs_initialize(ext2, device))
    {
        uprint("EXT2: an error occured in initialization. Bye!");

        return 1;
    }

    /* 5 = "ext2-" */
    if (!(ext2_device = malloc(5 + strlen(device_name) + 1)))
    {
        uprint("EXT2: Cannot allocate memory");

        return 1;
    }

    sprintf(ext2_device, "ext2-%s", device_name);

    /* FIXME: dynamically generate device name */
    ret = fiu_create(ext2_device, 0755, &ext2_ops, &ext2->fiu);

    if (ret < 0)
    {
        uprint("EXT2: fiu_create() failed");

        return 1;
    }

    /* 15 = "EXT2: Mounting ", 4 = " on " */
    char *buf = malloc(15 + strlen(device) + 4 + strlen(mount_pt) + 1);

    if (!buf)
    {
        uprint("EXT2: Out of memory");

        return 1;
    }

    sprintf(buf, "EXT2: Mounting %s on %s", device, mount_pt);

    uprint(buf);

    free(buf);

    ret = fiu_main(&ext2->fiu, mount_pt);

    if (ret < 0)
    {
        uprint("EXT2: fiu_main() failed");

        return 1;
    }

    return ret;
}

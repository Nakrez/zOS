#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <zos/print.h>

#include "fs.h"
#include "file.h"

static char *create_device_name(char *device)
{
    char *end_pos = strrchr(device, '/');
    char *device_name;
    char *final;

    if (!end_pos || !*(end_pos + 1))
        device_name = device;
    else
        device_name = end_pos + 1;

    /* 5 = "ext2-" */
    if (!(final = malloc(5 + strlen(device_name) + 1)))
        return NULL;

    sprintf(final, "ext2-%s", device_name);

    return final;
}

void *ext2_fill_private(struct fiu_internal *fiu, struct fiu_opts *opts)
{
    struct ext2fs *ext2;

    if (!opts->device)
        return NULL;

    if (!(ext2 = malloc(sizeof (struct ext2fs))))
        return NULL;

    ext2->fiu = fiu;

    if (!ext2fs_initialize(ext2, opts->device))
    {
        free(ext2);

        return NULL;
    }

    if (!(fiu->device_name = create_device_name(opts->device)))
        return NULL;

    return ext2;
}

static struct fiu_ops ext2_ops = {
    .fill_private = ext2_fill_private,
    .lookup = ext2fs_lookup,
    .stat = ext2fs_stat,
    .mount = ext2fs_mount,
    .open = ext2fs_open,
    .read = ext2fs_read,
    .getdirent = ext2fs_getdirent,
    .close = ext2fs_close,
};

int main(int argc, char *argv[])
{
    return fiu_main(argc, argv, &ext2_ops);
}

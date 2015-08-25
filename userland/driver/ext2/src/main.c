#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <zos/print.h>

#include "fs.h"
#include "file.h"

static struct fiu_ops ext2_ops = {
    .init = ext2fs_initialize,
    .lookup = ext2fs_lookup,
    .stat = ext2fs_stat,
    .mount = ext2fs_mount,
    .open = ext2fs_open,
    .read = ext2fs_read,
    .getdirent = ext2fs_getdirent,
    .close = ext2fs_close,
};

struct fiu_fs_super_ops ext2_super_ops = {
    .create = ext2fs_create,
};

struct fiu_fs ext2_fs = {
    .name = "ext2",
    .super_ops = &ext2_super_ops,
    .ops = &ext2_ops,
};

int main(int argc, char *argv[])
{
    return fiu_master_main(&ext2_fs, argc, argv);
}

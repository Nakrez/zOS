#include <stdlib.h>

#include <zos/print.h>

#include "fs.h"
#include "file.h"

static void ext2_root_remount(struct fiu_internal *fiu,
                              struct req_root_remount *req)
{
    (void) fiu;
    (void) req;
}



static struct fiu_ops ext2_ops = {
    .root_remount = ext2_root_remount,
    .lookup = ext2fs_lookup,
    .open = ext2fs_open,
    .close = ext2fs_close,
};

int main(void)
{
    int ret;
    struct ext2fs *ext2;

    if (!(ext2 = malloc(sizeof (struct ext2fs))))
    {
        uprint("EXT2: Cannot allocate memory");

        return 1;
    }

    ext2->fiu.private = ext2;

    if (!ext2fs_initialize(ext2, "/dev/ata-disk0"))
    {
        uprint("EXT2: an error occured in initialization. Bye!");

        return 1;
    }

    ret = fiu_create("ext2-ata-disk0", 0, 0, 0755, &ext2_ops, &ext2->fiu);

    if (ret < 0)
    {
        uprint("EXT2: fiu_create() failed");

        return 1;
    }

    uprint("EXT2: Mounting /dev/ata-disk0 on /");

    ret = fiu_main(&ext2->fiu, "/");

    if (ret < 0)
    {
        uprint("EXT2: fiu_main() failed");

        return 1;
    }

    return ret;
}

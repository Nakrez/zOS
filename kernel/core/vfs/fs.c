#include <kernel/vfs/fs.h>

static int fiu_lookup(struct mount_entry *root, const char *path, uint16_t uid,
                      uint16_t gid, struct resp_lookup *ret)
{
    (void)root;
    (void)path;
    (void)uid;
    (void)gid;
    (void)ret;

    return 0;
}

static int fiu_open(struct mount_entry *root, ino_t inode, uint16_t uid,
                    uint16_t gid, int flags, int mode)
{
    (void)root;
    (void)inode;
    (void)uid;
    (void)gid;
    (void)flags;
    (void)mode;

    return 0;
}

struct fs_ops fiu_ops = {
    .lookup = fiu_lookup,
    .open = fiu_open,
};

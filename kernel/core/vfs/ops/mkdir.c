#include <string.h>

#include <kernel/errno.h>

#include <kernel/vfs/vops.h>
#include <kernel/vfs/fs.h>

int vfs_mkdir(const char *path, int uid, int gid, mode_t mode)
{
    int ret;
    int path_size = strlen(path);
    ino_t inode;
    struct mount_entry *mount_pt;

    if ((ret = vfs_lookup(path, uid, gid, &inode, &mount_pt)) < 0)
        return ret;

    if (!mount_pt->ops->mkdir)
        return -ENOSYS;

    if (ret == path_size)
        return -EEXIST;

    return mount_pt->ops->mkdir(mount_pt, path + ret, inode, uid, gid, mode);
}

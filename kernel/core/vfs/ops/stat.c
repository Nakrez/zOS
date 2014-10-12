#include <string.h>

#include <kernel/errno.h>
#include <kernel/thread.h>

#include <kernel/vfs/vops.h>
#include <kernel/vfs/vfs.h>
#include <kernel/vfs/fs.h>
#include <kernel/vfs/vdevice.h>

int vfs_stat(struct thread *t, const char *path, struct stat *buf)
{
    int ret;
    int path_size = strlen(path);
    struct mount_entry *mount_pt;
    struct resp_lookup lookup;

    if ((ret = vfs_lookup(path, t->uid, t->gid, &lookup, &mount_pt)) < 0)
        return ret;

    if (path_size != lookup.processed)
        return -ENOENT;

    if (!mount_pt->ops->stat)
        return -ENOSYS;

    return mount_pt->ops->stat(mount_pt, t->uid, t->gid, lookup.inode, buf);
}

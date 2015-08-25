#include <string.h>

#include <kernel/errno.h>

#include <kernel/proc/thread.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/vfs/vops.h>
#include <kernel/fs/vfs/message.h>
#include <kernel/fs/vfs/mount.h>

int vfs_mkdir(struct thread *t, const char *path, mode_t mode)
{
    int ret;
    struct resp_lookup res;
    struct mount_entry *mount_pt;
    uid_t uid;
    gid_t gid;

    /* Kernel request */
    if (!t) {
        uid = 0;
        gid = 0;
    } else {
        uid = t->uid;
        gid = t->gid;
    }

    ret = vfs_lookup(t, path, &res, &mount_pt);
    if (ret < 0)
        return ret;

    if (!mount_pt->fi->parent->fs_ops->mkdir)
        return -ENOSYS;

    if ((size_t)ret == strlen(path))
        return -EEXIST;

    return mount_pt->fi->parent->fs_ops->mkdir(mount_pt, path + ret,
                                               res.inode.inode, uid, gid,
                                               mode);
}

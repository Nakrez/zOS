#include <string.h>

#include <kernel/errno.h>

#include <kernel/proc/thread.h>

#include <kernel/fs/vfs.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/vfs/vops.h>
#include <kernel/fs/vfs/message.h>
#include <kernel/fs/vfs/mount.h>

int vfs_stat(struct thread *t, const char *path, struct stat *buf)
{
    int ret;
    int path_size = strlen(path);
    struct mount_entry *mount_pt;
    struct resp_lookup lookup;
    uid_t uid;
    gid_t gid;

    /* Kernel request */
    if (!t)
    {
        uid = 0;
        gid = 0;
    }
    else
    {
        uid = t->uid;
        gid = t->gid;
    }

    if ((ret = vfs_lookup(t, path, &lookup, &mount_pt)) < 0)
        return ret;

    if (path_size != lookup.processed)
        return -ENOENT;

    if (!mount_pt->fs_ops->stat)
        return -ENOSYS;

    return mount_pt->fs_ops->stat(mount_pt, uid, gid, lookup.inode.inode, buf);
}

int vfs_fstat(struct thread *t, int fd, struct stat *buf)
{
    struct process *p;
    uid_t uid;
    gid_t gid;

    /* Kernel request */
    if (!t)
    {
        p = process_get(0);

        uid = 0;
        gid = 0;
    }
    else
    {
        p = t->parent;

        uid = t->uid;
        gid = t->gid;
    }

    if (fd < 0 || fd > PROCESS_MAX_OPEN_FD || !p->files[fd].used)
        return -EINVAL;

    if (!p->files[fd].mount->fs_ops->stat)
        return -ENOSYS;

    return p->files[fd].mount->fs_ops->stat(p->files[fd].mount, uid, gid,
                                            p->files[fd].inode->inode, buf);
}

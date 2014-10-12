#include <string.h>

#include <kernel/errno.h>
#include <kernel/thread.h>

#include <kernel/vfs/vops.h>
#include <kernel/vfs/vfs.h>
#include <kernel/vfs/fs.h>
#include <kernel/vfs/message.h>

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

int vfs_fstat(struct thread *t, int fd, struct stat *buf)
{
    struct process *p = t->parent;

    if (fd < 0 || fd > PROCESS_MAX_OPEN_FD || !p->files[fd].used)
        return -EINVAL;

    if (!p->files[fd].mount->ops->stat)
        return -ENOSYS;

    return p->files[fd].mount->ops->stat(p->files[fd].mount, t->uid, t->gid,
                                         p->files[fd].inode, buf);
}

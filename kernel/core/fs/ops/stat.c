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

    ret = vfs_lookup(t, path, &lookup, &mount_pt);
    if (ret < 0)
        return ret;

    if (strlen(path) != (size_t)lookup.processed)
        return -ENOENT;

    if (!mount_pt->fi->parent->fs_ops->stat)
        return -ENOSYS;

    return mount_pt->fi->parent->fs_ops->stat(mount_pt, uid, gid,
                                              lookup.inode.inode, buf);
}

int vfs_fstat(struct thread *t, int fd, struct stat *buf)
{
    int ret;
    struct process *p;
    uid_t uid;
    gid_t gid;
    struct file *file;

    /* Kernel request */
    if (!t) {
        p = process_get(0);

        uid = 0;
        gid = 0;
    } else {
        p = t->parent;

        uid = t->uid;
        gid = t->gid;
    }

    ret = process_file_from_fd(p, fd, &file);
    if (ret < 0)
        return ret;

    if (!file->mount->fi->parent->fs_ops->stat)
        return -ENOSYS;

    return file->mount->fi->parent->fs_ops->stat(file->mount, uid, gid,
                                                 file->inode->inode, buf);
}

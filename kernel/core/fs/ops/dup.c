#include <string.h>

#include <kernel/errno.h>

#include <kernel/proc/process.h>
#include <kernel/proc/thread.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/fs/vfs/vops.h>

static struct process *get_parent(struct thread *t)
{
    if (t)
        return t->parent;

    return process_get(0);
}

int vfs_dup(struct thread *t, int oldfd)
{
    struct process *p = get_parent(t);
    struct inode *new_inode;
    int newfd;

    if (!process_fd_exist(p, oldfd))
        return -EBADF;

    new_inode = kmalloc(sizeof (struct inode));
    if (!new_inode)
        return -ENOMEM;

    if ((newfd = process_new_fd(p)) < 0) {
        kfree(new_inode);
        return newfd;
    }

    /* XXX: Make inode shared */
    memcpy(new_inode, p->files[oldfd].inode, sizeof (struct inode));

    p->files[newfd].inode = new_inode;
    p->files[newfd].offset = p->files[oldfd].offset;
    p->files[newfd].mount = p->files[oldfd].mount;
    p->files[newfd].f_ops = p->files[oldfd].f_ops;

    return newfd;
}

int vfs_dup2(struct thread *t, int oldfd, int newfd)
{
    struct process *p = get_parent(t);
    struct inode *new_inode;

    if (!process_fd_exist(p, oldfd))
        return -EBADF;

    if (newfd < 0 || newfd >= PROCESS_MAX_OPEN_FD)
        return -EBADF;

    new_inode = kmalloc(sizeof (struct inode));
    if (!new_inode)
        return -ENOMEM;

    if (p->files[newfd].used)
    {
        int ret = vfs_close(t, newfd);

        if (ret < 0)
            return ret;
    }

    spinlock_lock(&p->files_lock);

    p->files[newfd].used = 1;

    spinlock_unlock(&p->files_lock);

    p->files[newfd].inode = p->files[oldfd].inode;
    p->files[newfd].offset = p->files[oldfd].offset;
    p->files[newfd].mount = p->files[oldfd].mount;
    p->files[newfd].f_ops = p->files[oldfd].f_ops;

    return newfd;
}

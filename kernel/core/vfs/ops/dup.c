#include <kernel/errno.h>

#include <kernel/proc/process.h>
#include <kernel/proc/thread.h>

#include <kernel/vfs/vops.h>

static struct process *get_parent(struct thread *t)
{
    if (t)
        return t->parent;

    return process_get(0);
}

int vfs_dup(struct thread *t, int oldfd)
{
    struct process *p = get_parent(t);
    int newfd;

    if (!process_fd_exist(p, oldfd))
        return -EBADF;

    if ((newfd = process_new_fd(p)) < 0)
        return newfd;

    p->files[newfd].mode = p->files[oldfd].mode;
    p->files[newfd].offset = p->files[oldfd].offset;
    p->files[newfd].inode = p->files[oldfd].inode;
    p->files[newfd].dev = p->files[oldfd].dev;
    p->files[newfd].mount= p->files[oldfd].mount;

    return newfd;
}

int vfs_dup2(struct thread *t, int oldfd, int newfd)
{
    struct process *p = get_parent(t);

    if (!process_fd_exist(p, oldfd))
        return -EBADF;

    if (newfd < 0 || newfd >= PROCESS_MAX_OPEN_FD)
        return -EBADF;

    if (p->files[newfd].used)
    {
        int ret = vfs_close(t, newfd);

        if (ret < 0)
            return ret;
    }

    spinlock_lock(&p->files_lock);

    p->files[newfd].used = 1;

    spinlock_unlock(&p->files_lock);

    p->files[newfd].mode = p->files[oldfd].mode;
    p->files[newfd].offset = p->files[oldfd].offset;
    p->files[newfd].inode = p->files[oldfd].inode;
    p->files[newfd].dev = p->files[oldfd].dev;
    p->files[newfd].mount= p->files[oldfd].mount;

    return newfd;
}

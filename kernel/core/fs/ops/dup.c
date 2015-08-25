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
    int ret;
    int newfd;
    struct process *p = get_parent(t);

    if (!process_fd_exist(p, oldfd))
        return -EBADF;

    newfd = process_new_fd(p);
    if (newfd < 0)
        return newfd;

    ret = process_dup_file(&p->files[newfd], &p->files[oldfd]);
    if (ret < 0) {
        process_free_fd(p, newfd);
        return ret;
    }

    return newfd;
}

int vfs_dup2(struct thread *t, int oldfd, int newfd)
{
    int ret;
    struct process *p = get_parent(t);

    if (!process_fd_exist(p, oldfd))
        return -EBADF;

    if (newfd < 0 || newfd >= PROCESS_MAX_OPEN_FD)
        return -EBADF;

    if (p->files[newfd].used) {
        ret = vfs_close(t, newfd);
        if (ret < 0)
            return ret;
    }

    spinlock_lock(&p->files_lock);

    p->files[newfd].used = 1;

    spinlock_unlock(&p->files_lock);

    ret = process_dup_file(&p->files[newfd], &p->files[oldfd]);
    if (ret < 0) {
        process_free_fd(p, newfd);
        return ret;
    }

    return newfd;
}

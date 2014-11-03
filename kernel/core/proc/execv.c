#include <kernel/errno.h>
#include <kernel/scheduler.h>

#include <kernel/vfs/vops.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/proc/elf.h>
#include <kernel/proc/process.h>
#include <kernel/proc/thread.h>

static int check_exec_perm(struct thread *thread, struct stat *stat)
{
    if (thread->uid == stat->st_uid)
        return stat->st_mode & 0100;
    else if (thread->gid == stat->st_gid)
        return stat->st_mode & 0010;

    return stat->st_mode & 0001;
}

int process_execv(struct thread *thread, const char *filename,
                  char *const argv[])
{
    (void)argv;

    int ret;
    int fd_binary;
    void *binary;
    char *buf;
    struct stat exe_stat;
    uintptr_t entry;

    if ((ret = vfs_stat(thread, filename, &exe_stat)) < 0)
        return ret;

    if (!check_exec_perm(thread, &exe_stat))
        return -EACCES;

    if (!(binary = kmalloc(exe_stat.st_size)))
        return -ENOMEM;

    if ((fd_binary = vfs_open(NULL, filename, 0, 0)) < 0)
    {
        kfree(binary);

        return fd_binary;
    }

    buf = binary;

    while (exe_stat.st_size > 0)
    {
        if ((ret = vfs_read(NULL, fd_binary, buf, 8192)) < 0)
        {
            kfree(binary);

            vfs_close(NULL, fd_binary);

            return ret;
        }

        exe_stat.st_size -= ret;
        buf += ret;
    }

    vfs_close(NULL, fd_binary);

    if (!is_elf(binary))
    {
        kfree(binary);

        return -ELIBBAD;
    }

    klist_for_each(&thread->parent->threads, tlist, list)
    {
        struct thread *t = klist_elem(tlist, struct thread, list);

        if (t != thread)
            thread_exit(thread);
    }

    as_clean(thread->parent->as);

    entry = process_load_elf(thread->parent, (uintptr_t)binary);

    kfree(binary);

    if (!entry)
    {
        scheduler_update(NULL);
        process_exit(thread->parent, 0);
    }

    thread_update_exec(thread, entry);

    scheduler_update(NULL);

    return 0;
}

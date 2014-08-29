#include <kernel/syscall.h>
#include <kernel/errno.h>
#include <kernel/thread.h>

#include <kernel/vfs/vfs.h>

int sys_vfs_device_create(struct syscall *interface)
{
    char *name = (void *)interface->arg1;
    int pid = thread_current()->parent->pid;
    int uid = interface->arg2;
    int gid = interface->arg3;
    int perm = interface->arg4;
    int ops = interface->arg5;

    /* If thread is not root don't event think about creating the device */
    if (thread_current()->uid != 0)
        return -EPERM;

    return vfs_device_create(name, pid, uid, gid, perm, ops);
}

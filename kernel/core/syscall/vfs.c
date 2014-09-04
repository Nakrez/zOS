#include <kernel/syscall.h>
#include <kernel/errno.h>
#include <kernel/thread.h>

#include <kernel/vfs/vfs.h>
#include <kernel/vfs/vops.h>
#include <kernel/vfs/vdevice.h>

int sys_vfs_device_create(struct syscall *interface)
{
    /* FIXME: Check name */
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

int sys_vfs_device_recv_request(struct syscall *interface)
{
    /* FIXME: Check buf */
    int dev = interface->arg1;
    char *buf = (void *)interface->arg2;
    size_t size = interface->arg3;

    if (!as_is_mapped(thread_current()->parent->as, (vaddr_t) buf, size))
        return -EFAULT;

    return device_recv_request(dev, buf, size);
}

int sys_vfs_device_send_response(struct syscall *interface)
{
    /* FIXME: Check buf */
    int dev = interface->arg1;
    char *buf = (void *)interface->arg2;
    size_t size = interface->arg3;

    if (!as_is_mapped(thread_current()->parent->as, (vaddr_t) buf, size))
        return -EFAULT;

    return device_send_response(dev, buf, size);
}

int sys_vfs_open(struct syscall *interface)
{
    /* FIXME: Check pathname */
    const char *pathname = (void *)interface->arg1;
    int flags = interface->arg2;
    int mode = interface->arg3;

    return vfs_open(pathname, flags, mode);
}

int sys_vfs_read(struct syscall *interface)
{
    int fd = interface->arg1;
    void *buf = (void *)interface->arg2;
    size_t count = interface->arg3;

    if (!as_is_mapped(thread_current()->parent->as, (vaddr_t) buf, count))
        return -EFAULT;

    return vfs_read(fd, buf, count);
}

int sys_vfs_write(struct syscall *interface)
{
    int fd = interface->arg1;
    const void *buf = (void *)interface->arg2;
    size_t count = interface->arg3;

    if (!as_is_mapped(thread_current()->parent->as, (vaddr_t) buf, count))
        return -EFAULT;

    return vfs_write(fd, buf, count);
}

int sys_vfs_close(struct syscall *interface)
{
    int fd = interface->arg1;

    return vfs_close(fd);
}

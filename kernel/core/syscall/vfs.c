#include <kernel/syscall.h>
#include <kernel/errno.h>
#include <kernel/thread.h>

#include <kernel/vfs/vfs.h>
#include <kernel/vfs/mount.h>
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
    uint32_t req_id = interface->arg2;
    char *buf = (void *)interface->arg3;
    size_t size = interface->arg4;

    if (!as_is_mapped(thread_current()->parent->as, (vaddr_t) buf, size))
        return -EFAULT;

    return device_send_response(dev, req_id, buf, size);
}

int sys_vfs_open(struct syscall *interface)
{
    /* FIXME: Check pathname */
    struct thread *thread = thread_current();
    const char *pathname = (void *)interface->arg1;
    int flags = interface->arg2;
    int mode = interface->arg3;

    return vfs_open(pathname, thread->uid, thread->gid, flags, mode);
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

int sys_vfs_lseek(struct syscall *interface)
{
    int fd = interface->arg1;
    int off = interface->arg2;
    int whence = interface->arg3;

    return vfs_lseek(fd, off, whence);
}

int sys_vfs_mount(struct syscall *interface)
{
    int fd = interface->arg1;
    const char *path = (char *)interface->arg2;

    return vfs_mount(fd, path);
}

int sys_vfs_stat(struct syscall *interface)
{
    const char *path = (char *)interface->arg1;
    struct stat *buf = (void *)interface->arg2;

    if (!as_is_mapped(thread_current()->parent->as, (vaddr_t) buf,
                      sizeof (struct stat)))
        return -EFAULT;

    return vfs_stat(thread_current(), path, buf);
}

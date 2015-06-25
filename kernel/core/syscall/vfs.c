#include <kernel/syscall.h>
#include <kernel/errno.h>

#include <kernel/proc/thread.h>

#include <kernel/fs/vfs.h>

#include <kernel/fs/vfs/mount.h>
#include <kernel/fs/vfs/vops.h>
#include <kernel/fs/vfs/vdevice.h>

int sys_vfs_device_create(struct syscall *interface)
{
    /* FIXME: Check name */
    char *name = (void *)interface->arg1;
    pid_t pid = thread_current()->parent->pid;
    int perm = interface->arg2;
    int ops = interface->arg3;

    /* If thread is not root don't event think about creating the device */
    if (thread_current()->uid != 0)
        return -EPERM;

    return vfs_device_create(name, pid, perm, ops);
}

int sys_vfs_device_recv_request(struct syscall *interface)
{
    /* FIXME: Check buf */
    dev_t dev = interface->arg1;
    char *buf = (void *)interface->arg2;
    size_t size = interface->arg3;

    if (!as_is_mapped(thread_current()->parent->as, (vaddr_t) buf, size))
        return -EFAULT;

    return device_recv_request(dev, buf, size);
}

int sys_vfs_device_send_response(struct syscall *interface)
{
    /* FIXME: Check buf */
    dev_t dev = interface->arg1;
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
    const char *pathname = (void *)interface->arg1;
    int flags = interface->arg2;
    int mode = interface->arg3;

    return vfs_open(thread_current(), pathname, flags, mode);
}

int sys_vfs_read(struct syscall *interface)
{
    int fd = interface->arg1;
    void *buf = (void *)interface->arg2;
    size_t count = interface->arg3;

    if (!as_is_mapped(thread_current()->parent->as, (vaddr_t) buf, count))
        return -EFAULT;

    return vfs_read(thread_current(), fd, buf, count);
}

int sys_vfs_write(struct syscall *interface)
{
    int fd = interface->arg1;
    const void *buf = (void *)interface->arg2;
    size_t count = interface->arg3;

    if (!as_is_mapped(thread_current()->parent->as, (vaddr_t) buf, count))
        return -EFAULT;

    return vfs_write(thread_current(), fd, buf, count);
}

int sys_vfs_close(struct syscall *interface)
{
    int fd = interface->arg1;

    return vfs_close(thread_current(), fd);
}

int sys_vfs_lseek(struct syscall *interface)
{
    int fd = interface->arg1;
    uint64_t off_high = (uint64_t)interface->arg2 & 0xFFFFFFFF;
    uint64_t off_low = (uint64_t)interface->arg3 & 0xFFFFFFFF;
    off_t off = off_high << 32 | off_low;
    int whence = interface->arg4;

    return vfs_lseek(thread_current(), fd, off, whence);
}

int sys_vfs_mount(struct syscall *interface)
{
    dev_t dev = interface->arg1;
    const char *path = (char *)interface->arg2;

    return vfs_mount(thread_current(), dev, path);
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

int sys_vfs_fstat(struct syscall *interface)
{
    int fd = interface->arg1;
    struct stat *buf = (void *)interface->arg2;

    if (!as_is_mapped(thread_current()->parent->as, (vaddr_t) buf,
                      sizeof (struct stat)))
        return -EFAULT;

    return vfs_fstat(thread_current(), fd, buf);
}

int sys_vfs_ioctl(struct syscall *interface)
{
    int fd = interface->arg1;
    int request = interface->arg2;
    int *argp = (int *)interface->arg3;

    return vfs_ioctl(thread_current(), fd, request, argp);
}

int sys_vfs_dup(struct syscall *interface)
{
    int oldfd = interface->arg1;

    return vfs_dup(thread_current(), oldfd);
}

int sys_vfs_dup2(struct syscall *interface)
{
    int oldfd = interface->arg1;
    int newfd = interface->arg2;

    return vfs_dup2(thread_current(), oldfd, newfd);
}

int sys_vfs_getdirent(struct syscall *interface)
{
    int fd = interface->arg1;
    struct dirent *dirent = (void *)interface->arg2;
    int index = interface->arg3;

    return vfs_getdirent(thread_current(), fd, dirent, index);
}

int sys_vfs_device_exists(struct syscall *interface)
{
    char *device = (void *)interface->arg1;

    return device_exists(device);
}

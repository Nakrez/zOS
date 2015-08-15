#include <kernel/syscall.h>
#include <kernel/errno.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/proc/thread.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/fiu.h>
#include <kernel/fs/channel.h>

#include <kernel/fs/vfs/mount.h>
#include <kernel/fs/vfs/vops.h>
#include <kernel/fs/vfs/device.h>

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

    return vfs_device_create(name, pid, perm, ops, &fiu_f_ops);
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

int sys_vfs_open_device(struct syscall *interface)
{
    const char *device_name = (void *)interface->arg1;
    int flags = interface->arg2;
    int mode = interface->arg3;

    return vfs_open_device(thread_current(), device_name, flags, mode);
}

int sys_fs_channel_create(struct syscall *interface)
{
    int fd;
    int err;
    const char *name = (void *)interface->arg1;
    struct file *file;
    struct process *p = thread_current()->parent;

    fd = process_new_fd(p);
    if (fd < 0)
        return fd;

    file = &p->files[fd];

    err = channel_create(name, file, NULL);
    if (err < 0) {
        process_free_fd(p, fd);
        return err;
    }

    return fd;
}

int sys_fs_channel_open(struct syscall *interface)
{
    int fd;
    int err;
    const char *name = (void *)interface->arg1;
    struct file *file;
    struct process *p = thread_current()->parent;

    fd = process_new_fd(p);
    if (fd < 0)
        return fd;

    file = &p->files[fd];

    err = channel_open_from_name(name, file, NULL);
    if (err < 0) {
        process_free_fd(p, fd);
        return err;
    }

    return fd;
}

/* User interface is fs_register(name, channel_fd, ops) */
int sys_fs_register(struct syscall *interface)
{
    const char *name = (void *)interface->arg1;
    int channel_fd = interface->arg2;
    vop_t ops = interface->arg3;
    struct process *p = thread_current()->parent;
    struct file *file;
    struct fiu_fs *priv;

    priv = kmalloc(sizeof (struct fiu_fs));
    if (!priv)
        return -ENOMEM;

    if (!process_fd_exist(p, channel_fd)) {
        kfree(priv);
        return -EBADF;
    }

    spinlock_lock(&p->files_lock);

    file = &p->files[channel_fd];

    if (file->f_ops != &channel_master_f_ops) {
        spinlock_unlock(&p->files_lock);
        kfree(priv);
        return -EBADF;
    }

    priv->master = file->private;
    spinlock_unlock(&p->files_lock);

    /* XXX: Check ops */
    priv->ops = ops;

    return fs_register(name, p->pid, &fiu_fs_super_ops, &fiu_fs_ops,
                       &fiu_f_ops, priv);
}

int sys_fs_unregister(struct syscall *interface)
{
    const char *name = (void *) interface->arg1;
    pid_t pid = thread_current()->parent->pid;

    return fs_unregister(name, pid);
}

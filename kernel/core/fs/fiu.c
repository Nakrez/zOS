#include <string.h>

#include <kernel/errno.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/proc/process.h>
#include <kernel/proc/thread.h>

#include <kernel/fs/fiu.h>
#include <kernel/fs/channel.h>
#include <kernel/fs/vfs.h>

#include <kernel/fs/vfs/message.h>
#include <kernel/fs/vfs/mount.h>

static int fiu_channel_read_rw(struct channel_slave *slave, void *buf_in,
                               size_t size_in, void *buf_out, size_t size_out)
{
    int ret;

    ret = channel_slave_write(slave, buf_in, size_in);
    if (ret < 0)
        return ret;

    ret = channel_slave_read(slave, buf_out, size_out);
    if (ret < 0)
        return ret;

    return 0;
}

static int fiu_lookup(struct mount_entry *root, const char *path, uid_t uid,
                      gid_t gid, struct resp_lookup *resp)
{
    int ret;
    int path_empty = 0;
    struct req_lookup req;
    struct channel_slave *slave;
    struct process *pdevice;
    struct fiu_fs_instance *fi = root->fi->private;

    ret = channel_open(fi->channel, NULL, &slave);
    if (ret < 0)
        return ret;

    if (!strcmp("", path)) {
        path_empty = 1;
        path = "/";
    }

    pdevice = fi->channel->proc;

    req.path_size = strlen(path);
    req.path = (void *)as_map(pdevice->as, 0, 0, req.path_size,
                              AS_MAP_USER | AS_MAP_WRITE);
    if (!req.path) {
        ret = -ENOMEM;
        goto error;
    }

    ret = as_copy(thread_current()->parent->as, pdevice->as, path,
                  req.path, req.path_size + 1);
    if (ret < 0)
        goto error_unmap;

    req.uid = uid;
    req.gid = gid;

    req.hdr.op = VFS_LOOKUP;
    req.hdr.slave_id = slave->id;

    ret = fiu_channel_read_rw(slave, &req, sizeof (req), resp, sizeof (*resp));
    if (ret < 0)
        goto error_unmap;

    channel_slave_close(slave);

    if (path_empty)
        resp->processed = 0;

    as_unmap(pdevice->as, (vaddr_t)req.path, AS_UNMAP_RELEASE);

    if (resp->ret < 0)
        return resp->ret;

    return 0;

error_unmap:
    as_unmap(pdevice->as, (vaddr_t)req.path, AS_UNMAP_RELEASE);
error:
    channel_slave_close(slave);
    return ret;
}

static int fiu_stat(struct mount_entry *root, uid_t uid, gid_t gid,
                    ino_t inode, struct stat *buf)
{
    int ret;
    struct channel_slave *slave;
    struct req_stat req;
    struct resp_stat resp;
    struct fiu_fs *fs = root->fi->parent->private;
    struct fiu_fs_instance *fi = root->fi->private;

    if (!(fs->ops & VFS_OPS_GETDIRENT))
        return -ENOSYS;

    ret = channel_open(fi->channel, NULL, &slave);
    if (ret < 0)
        return ret;

    req.uid = uid;
    req.gid = gid;
    req.inode = inode;

    req.hdr.op = VFS_STAT;
    req.hdr.slave_id = slave->id;

    ret = fiu_channel_read_rw(slave, &req, sizeof (req), &resp, sizeof (resp));
    if (ret < 0) {
        channel_slave_close(slave);
        return ret;
    }

    channel_slave_close(slave);

    ret = resp.ret;

    if (ret < 0)
        return ret;

    memcpy(buf, &resp.stat, sizeof (struct stat));

    return ret;
}

static int fiu_mount(struct mount_entry *root, ino_t inode, int mount_nb)
{
    int ret;
    struct channel_slave *slave;
    struct req_mount req;
    struct resp_mount resp;
    struct fiu_fs *fs = root->fi->parent->private;
    struct fiu_fs_instance *fi = root->fi->private;

    if (!(fs->ops & VFS_OPS_MOUNT))
        return -ENOSYS;

    ret = channel_open(fi->channel, NULL, &slave);
    if (ret < 0)
        return ret;

    req.inode = inode;
    req.mount_nb = mount_nb;

    req.hdr.op = VFS_MOUNT;
    req.hdr.slave_id = slave->id;

    ret = fiu_channel_read_rw(slave, &req, sizeof (req), &resp, sizeof (resp));

    channel_slave_close(slave);

    if (ret < 0)
        return ret;

    return resp.ret;
}

static int fiu_getdirent(struct mount_entry *root, ino_t inode,
                         struct dirent *d, int index)
{
    int ret;
    struct req_getdirent req;
    struct resp_getdirent resp;
    struct channel_slave *slave;
    struct fiu_fs *fs = root->fi->parent->private;
    struct fiu_fs_instance *fi = root->fi->private;

    if (!(fs->ops & VFS_OPS_GETDIRENT))
        return -ENOSYS;

    ret = channel_open(fi->channel, NULL, &slave);
    if (ret < 0)
        return ret;

    req.inode = inode;
    req.index = index;

    req.hdr.slave_id = slave->id;
    req.hdr.op = VFS_GETDIRENT;

    ret = fiu_channel_read_rw(slave, &req, sizeof (req), &resp, sizeof (resp));

    channel_slave_close(slave);

    if (ret < 0)
        return ret;

    if (resp.ret < 0)
        return resp.ret;

    memcpy(d, &resp.dirent, sizeof (struct dirent));

    return resp.ret;
}

static int fiu_open(struct file *file, ino_t inode, pid_t pid, uid_t uid,
                    gid_t gid, int flags, mode_t mode)
{
    int ret = 0;
    struct req_open req;
    struct resp_open resp;
    struct channel_slave *slave;
    struct fiu_file_private *priv;

    priv = kmalloc(sizeof (struct fiu_file_private));
    if (!priv)
        return -ENOMEM;

    /* This file belongs to a file system */
    if (file->mount) {
        struct fiu_fs *fs = file->mount->fi->parent->private;
        struct fiu_fs_instance *fi = file->mount->fi->private;

        ret = channel_open(fi->channel, NULL, &slave);
        if (ret < 0)
            goto error;

        priv->ops = fs->ops;
    } else { /* This is a device */
        struct device *device;
        struct fiu_device_private *dev_priv;

        device = device_get(file->inode->dev);

        dev_priv = device->private;

        ret = channel_open(dev_priv->master, NULL, &slave);
        if (ret < 0)
            goto error;

        priv->ops = device->ops;
    }

    req.inode = inode;
    req.uid = uid;
    req.gid = gid;
    req.flags = flags;
    req.mode = mode;
    req.pid = pid;

    req.hdr.slave_id = slave->id;
    req.hdr.op = VFS_OPEN;

    ret = fiu_channel_read_rw(slave, &req, sizeof (req), &resp, sizeof (resp));
    if (ret < 0)
        goto error_channel;

    if (resp.ret < 0) {
        ret = resp.ret;
        goto error_channel;
    }

    priv->slave = slave;

    file->private = priv;

    return resp.inode;

error_channel:
    channel_slave_close(slave);
error:
    kfree(priv);
    return ret;
}

static int fiu_read_write(struct file *file, struct process *p,
                          struct req_rdwr *req, void *buf, int op)
{
    int ret;
    struct fiu_file_private *private = file->private;
    struct process *pdevice;
    struct resp_rdwr resp;

    if (!(private->ops & op))
        return -ENOSYS;

    pdevice = private->slave->parent->proc;

    req->hdr.op = op;
    req->hdr.slave_id = private->slave->id;

    req->data = (void *)as_map(pdevice->as, 0, 0, req->size,
                               AS_MAP_USER | AS_MAP_WRITE);
    if (!req->data)
        return -ENOMEM;

    if (op == VFS_WRITE) {
        ret = as_copy(p->as, pdevice->as, buf, req->data, req->size);
        if (ret < 0)
            goto end;
    }

    ret = fiu_channel_read_rw(private->slave, req, sizeof (*req), &resp,
                              sizeof (resp));
    if (ret < 0)
        goto end;

    if (resp.ret < 0) {
        ret = resp.ret;
        goto end;
    }

    if (op == VFS_READ)
        ret = as_copy(pdevice->as, p->as, req->data, buf, resp.size);

    if (ret == 0) {
        ret = resp.size;
        req->off += ret;
    }

end:
    as_unmap(pdevice->as, (vaddr_t)req->data, AS_UNMAP_RELEASE);

    return ret;
}

static int fiu_read(struct file *file, struct process *p, struct req_rdwr *req,
                    void *buf)
{
    return fiu_read_write(file, p, req, buf, VFS_READ);
}
static int fiu_write(struct file *file, struct process *p,
                     struct req_rdwr *req, void *buf)
{
    return fiu_read_write(file, p, req, buf, VFS_WRITE);
}

static int fiu_ioctl(struct file *file, struct req_ioctl *req, int *argp)
{
    int ret;
    struct resp_ioctl resp;
    struct fiu_file_private *private = file->private;

    req->hdr.op = VFS_IOCTL;
    req->hdr.slave_id = private->slave->id;

    ret = fiu_channel_read_rw(private->slave, req, sizeof (*req), &resp,
                              sizeof (resp));
    if (ret < 0)
        return ret;

    if (resp.ret < 0)
        return resp.ret;

    if (argp && resp.modify_argp)
        *argp = resp.argp;

    return 0;
}

static int fiu_dup(struct file *new, struct file *old)
{
    int ret;
    struct fiu_file_private *old_priv = old->private;
    struct fiu_file_private *new_priv;
    struct channel_slave *slave;

    new_priv = kmalloc(sizeof (struct fiu_file_private));
    if (!new_priv)
        return -ENOMEM;

    ret = channel_open(old_priv->slave->parent, NULL, &slave);
    if (ret < 0) {
        kfree(new_priv);
        return ret;
    }

    new_priv->slave = slave;
    new_priv->ops = old_priv->ops;

    new->private = new_priv;

    return 0;
}

static int fiu_close(struct file *file, ino_t inode)
{
    int ret;
    struct req_close req;
    struct resp_close resp;
    struct fiu_file_private *private = file->private;

    req.inode = inode;

    req.hdr.op = VFS_CLOSE;
    req.hdr.slave_id = private->slave->id;

    ret = fiu_channel_read_rw(private->slave, &req, sizeof (req), &resp,
                              sizeof (resp));

    channel_slave_close(private->slave);
    kfree(private);

    if (ret < 0)
        return ret;

    return resp.ret;
}

static int fiu_create(struct fs_instance *fi, const char *device,
                      const char *mount_pt)
{
    (void) mount_pt;

    int ret;
    dev_t dev_id;
    struct fiu_fs *fs = fi->parent->private;
    struct fiu_fs_instance *priv;
    struct channel_slave *slave;
    struct resp_fs_create resp;
    struct req_fs_create req;
    struct channel *channel;

    dev_id = device_get_from_name(device);
    if (dev_id < 0)
        return dev_id;

    priv = kmalloc(sizeof (struct fiu_fs_instance));
    if (!priv)
        return -ENOMEM;

    ret = channel_open(fs->master, NULL, &slave);
    if (ret < 0)
        goto error_channel;

    req.hdr.slave_id = slave->id;
    req.hdr.op = VFS_FS_CREATE;
    strncpy(req.device, device, VFS_DEV_MAX_NAMEL);

    ret = fiu_channel_read_rw(slave, &req, sizeof (req), &resp, sizeof (resp));
    if (ret < 0)
        goto error;

    if (resp.ret < 0) {
        ret = resp.ret;
        goto error;
    }

    channel = channel_from_name(resp.device);
    if (!channel) {
        ret = -EINVAL;
        goto error;
    }

    channel_slave_close(slave);

    fi->private = priv;
    priv->channel = channel;

    return 0;

error:
    channel_slave_close(slave);
error_channel:
    kfree(priv);
    return ret;
}

struct fs_operation fiu_fs_ops = {
    .lookup = fiu_lookup,
    .stat = fiu_stat,
    .mount = fiu_mount,
    .getdirent = fiu_getdirent,
};

struct file_operation fiu_f_ops = {
    .open = fiu_open,
    .read = fiu_read,
    .write = fiu_write,
    .ioctl = fiu_ioctl,
    .dup = fiu_dup,
    .close = fiu_close,
};

struct fs_super_operation fiu_fs_super_ops = {
    .create = fiu_create,
};

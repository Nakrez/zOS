#include <string.h>

#include <kernel/errno.h>

#include <kernel/proc/process.h>
#include <kernel/proc/thread.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/vfs/device.h>

static int fiu_lookup(struct mount_entry *root, const char *path, uid_t uid,
                      gid_t gid, struct resp_lookup *ret)
{
    int res;
    int path_empty = 0;
    struct device *device;
    struct message *message;
    struct message *response;
    struct req_lookup *request;
    struct resp_lookup *answer;
    struct process *pdevice;

    if (!(device = device_get(root->dev)))
        return -ENODEV;

    if (!(message = message_alloc(sizeof (struct req_lookup))))
        return -ENOMEM;

    if (!strcmp("", path))
    {
        path_empty = 1;
        path = "/";
    }

    request = MESSAGE_EXTRACT(struct req_lookup, message);

    pdevice = process_get(device->pid);

    request->path_size = strlen(path);
    request->path = (void *)as_map(pdevice->as, 0, 0, request->path_size,
                                   AS_MAP_USER | AS_MAP_WRITE);

    if (!request->path)
    {
        message_free(message);

        return -ENOMEM;
    }

    res = as_copy(thread_current()->parent->as, pdevice->as, path,
                  request->path, request->path_size + 1);

    if (res < 0)
    {
        as_unmap(pdevice->as, (vaddr_t)request->path, AS_UNMAP_RELEASE);

        message_free(message);

        return res;
    }

    request->uid = uid;
    request->gid = gid;

    message->mid = (message->mid & ~0xFF) | VFS_LOOKUP;

    if ((res = channel_send_recv(device->channel, message, &response)) < 0)
    {
        as_unmap(pdevice->as, (vaddr_t)request->path, AS_UNMAP_RELEASE);

        message_free(message);

        return res;
    }

    answer = MESSAGE_EXTRACT(struct resp_lookup, response);

    ret->ret = answer->ret;
    ret->inode = answer->inode;

    if (path_empty)
        ret->processed = 0;
    else
        ret->processed = answer->processed;

    ret->dev = answer->dev;

    message_free(message);
    message_free(response);
    as_unmap(pdevice->as, (vaddr_t)request->path, AS_UNMAP_RELEASE);

    if (ret->ret < 0)
        return ret->ret;

    return 0;
}

static int fiu_stat(struct mount_entry *root, uid_t uid, gid_t gid,
                    ino_t inode, struct stat *buf)
{
    int res;
    struct device *device;
    struct message *message;
    struct message *response;
    struct req_stat *request;
    struct resp_stat *answer;

    if (!(device = device_get(root->dev)))
        return -ENODEV;

    if (!(device->ops & VFS_OPS_STAT))
        return -ENOSYS;

    if (!(message = message_alloc(sizeof (struct req_stat))))
        return -ENOMEM;

    request = MESSAGE_EXTRACT(struct req_stat, message);

    request->uid = uid;
    request->gid = gid;
    request->inode = inode;

    message->mid = (message->mid & ~0xFF) | VFS_STAT;

    if ((res = channel_send_recv(device->channel, message, &response)) < 0)
    {
        message_free(message);

        return res;
    }

    answer = MESSAGE_EXTRACT(struct resp_stat, response);

    res = answer->ret;

    if (res < 0)
        goto end;

    memcpy(buf, &answer->stat, sizeof (struct stat));

end:
    message_free(message);
    message_free(response);

    return res;
}

static int fiu_mount(struct mount_entry *root, ino_t inode, int mount_nb)
{
    int res;
    struct device *device;
    struct message *message;
    struct message *response;
    struct req_mount *request;
    struct resp_mount *answer;

    if (!(device = device_get(root->dev)))
        return -ENODEV;

    if (!(device->ops & VFS_OPS_MOUNT))
        return -ENOSYS;

    if (!(message = message_alloc(sizeof (struct req_mount))))
        return -ENOMEM;

    request = MESSAGE_EXTRACT(struct req_mount, message);

    request->inode = inode;
    request->mount_nb = mount_nb;

    message->mid = (message->mid & ~0xFF) | VFS_MOUNT;

    if ((res = channel_send_recv(device->channel, message, &response)) < 0)
    {
        message_free(message);

        return res;
    }

    answer = MESSAGE_EXTRACT(struct resp_mount, response);

    res = answer->ret;

    message_free(message);
    message_free(response);

    return res;
}

static int fiu_open(struct mount_entry *root, ino_t inode, pid_t pid,
                    uid_t uid, gid_t gid, int flags, mode_t mode)
{
    return device_open(root->dev, inode, pid, uid, gid, flags, mode);
}

static int fiu_read(struct mount_entry *root, struct process *process,
                    struct req_rdwr *req, void *buf)
{
    return device_read_write(process, root->dev, req, buf, VFS_READ);
}

static int fiu_getdirent(struct mount_entry *root, ino_t inode,
                         struct dirent *d, int index)
{
    int res;
    struct device *device;
    struct message *message;
    struct message *response;
    struct req_getdirent *request;
    struct resp_getdirent *answer;

    if (!(device = device_get(root->dev)))
        return -ENODEV;

    if (!(device->ops & VFS_OPS_GETDIRENT))
        return -ENOSYS;

    if (!(message = message_alloc(sizeof (struct req_getdirent))))
        return -ENOMEM;

    request = MESSAGE_EXTRACT(struct req_getdirent, message);

    request->inode = inode;
    request->index = index;

    message->mid = (message->mid & ~0xFF) | VFS_GETDIRENT;

    if ((res = channel_send_recv(device->channel, message, &response)) < 0)
    {
        message_free(message);

        return res;
    }

    answer = MESSAGE_EXTRACT(struct resp_getdirent, response);

    res = answer->ret;

    if (res < 0)
        goto end;

    memcpy(d, &answer->dirent, sizeof (struct dirent));

end:

    message_free(message);
    message_free(response);

    return res;
}

static int fiu_close(struct mount_entry *root, ino_t inode)
{
    return device_close(root->dev, inode);
}

struct fs_ops fiu_ops = {
    .lookup = fiu_lookup,
    .stat = fiu_stat,
    .mount = fiu_mount,
    .open = fiu_open,
    .read = fiu_read,
    .getdirent = fiu_getdirent,
    .close = fiu_close,
};

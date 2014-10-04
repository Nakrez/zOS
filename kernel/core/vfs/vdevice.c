#include <string.h>

#include <kernel/errno.h>
#include <kernel/kmalloc.h>
#include <kernel/thread.h>

#include <kernel/vfs/vfs.h>
#include <kernel/vfs/vdevice.h>

#include <arch/spinlock.h>

static struct vdevice devices[VFS_MAX_DEVICE];
static spinlock_t device_lock = SPINLOCK_INIT;

int device_create(int pid, const char __user* name, int ops,
                  struct vdevice **device)
{
    int found = 0;

    if (!(ops & VFS_OPS_OPEN) || !(ops & VFS_OPS_CLOSE))
        return -EINVAL;

    *device = NULL;

    spinlock_lock(&device_lock);

    for (int i = 0; i < VFS_MAX_DEVICE; ++i)
    {
        /* FIXME: User access */
        if (devices[i].active && strcmp(devices[i].name, name) == 0)
            found = 1;
        if (!(*device) && !devices[i].active)
        {
            if (!(devices[i].name = kmalloc(strlen(name) + 1)))
            {
                spinlock_unlock(&device_lock);

                return -ENOMEM;
            }

            if (!(devices[i].channel = channel_create()))
            {
                kfree(devices[i].name);
                spinlock_unlock(&device_lock);

                return -ENOMEM;
            }

            /* FIXME: User access !!! */
            strcpy(devices[i].name, name);

            devices[i].id = i;
            devices[i].active = 1;
            devices[i].pid = pid;
            devices[i].ops = ops;

            *device = &devices[i];
        }
    }

    /* Device already exists */
    if (found && *device)
    {
        (*device)->active = 0;
        kfree((*device)->name);
        channel_destroy((*device)->channel);
        *device = NULL;

        spinlock_unlock(&device_lock);

        return -EEXIST;
    }

    spinlock_unlock(&device_lock);

    if (!(*device))
        return -ENOMEM;

    return (*device)->id;
}

struct vdevice *device_get(int dev)
{
    if (dev < 0 || dev >= VFS_MAX_DEVICE)
        return NULL;

    if (!devices[dev].active)
        return NULL;

    return &devices[dev];
}

int device_recv_request(int dev, char *buf, size_t size)
{
    int pid = thread_current()->parent->pid;

    if (dev < 0 || dev >= VFS_MAX_DEVICE)
        return -EINVAL;

    if (!devices[dev].active)
        return -ENODEV;

    if (devices[dev].pid != pid)
        return -EINVAL;

    return channel_recv_request(devices[dev].channel, buf, size);
}

int device_send_response(int dev, uint32_t req_id, char *buf, size_t size)
{
    int res;
    int pid = thread_current()->parent->pid;
    struct message *message;

    if (dev < 0 || dev >= VFS_MAX_DEVICE)
        return -EINVAL;

    if (!devices[dev].active)
        return -ENODEV;

    if (devices[dev].pid != pid)
        return -EINVAL;

    if (!(message = message_alloc(size)))
        return -ENOMEM;

    memcpy(message + 1, buf, size);

    message->size = size;

    res = channel_send_response(devices[dev].channel, req_id, message);

    if (res < 0)
        message_free(message);

    return res;
}

int device_open(int dev, ino_t inode, uint16_t uid, uint16_t gid, int flags,
                mode_t mode)
{
    int ret = 0;
    struct vdevice *device;
    struct message *message;
    struct message *response;
    struct req_open *request;
    struct resp_open *answer;

    if (!(device = device_get(dev)))
        return -ENODEV;

    if (!(message = message_alloc(sizeof (struct req_open))))
        return -ENOMEM;

    request = MESSAGE_EXTRACT(struct req_open, message);

    request->inode = inode;
    request->uid = uid;
    request->gid = gid;
    request->flags = flags;
    request->mode = mode;

    message->mid = (message->mid & ~0xFF) | VFS_OPS_OPEN;

    if ((ret = channel_send_recv(device->channel, message, &response)) < 0)
    {
        message_free(message);

        return ret;
    }

    message_free(message);

    answer = MESSAGE_EXTRACT(struct resp_open, response);

    if (answer->ret < 0)
    {
        message_free(response);

        return answer->ret;
    }

    ret = answer->inode;

    message_free(response);

    return ret;
}

int device_destroy(int pid, int dev)
{
    if (dev < 0 || dev >= VFS_MAX_DEVICE)
        return -EINVAL;

    if (!devices[dev].active)
        return -ENODEV;

    if (devices[dev].pid != pid)
        return -EINVAL;

    spinlock_lock(&device_lock);

    devices[dev].active = 0;

    kfree(devices[dev].name);

    spinlock_unlock(&device_lock);

    return 0;
}

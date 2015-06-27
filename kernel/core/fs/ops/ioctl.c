#include <kernel/errno.h>

#include <kernel/proc/thread.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/vfs/message.h>
#include <kernel/fs/vfs/device.h>

int vfs_ioctl(struct thread *t, int fd, int req, int *argp)
{
    int ret;
    struct process *p;
    struct device *device;
    struct message *message;
    struct message *response;
    struct req_ioctl *request;
    struct resp_ioctl *answer;

    /* Kernel request */
    if (!t)
        p = process_get(0);
    else
        p = t->parent;

    if (fd < 0 || fd > PROCESS_MAX_OPEN_FD || !p->files[fd].used)
        return -EBADF;

    if (p->files[fd].dev < 0)
        return -ENOTTY;

    if (!(device = device_get(p->files[fd].dev)))
        return -ENODEV;

    if (!(device->ops & VFS_OPS_IOCTL))
        return -ENOSYS;

    if (!(message = message_alloc(sizeof (struct req_ioctl))))
        return -ENOMEM;

    request = MESSAGE_EXTRACT(struct req_ioctl, message);

    request->inode = p->files[fd].inode;
    request->request = req;
    request->with_argp = argp != NULL;

    if (argp)
        request->argp = *argp;

    message->mid = (message->mid & ~0xFF) | VFS_IOCTL;

    if ((ret = channel_send_recv(device->channel, message, &response)) < 0)
    {
        message_free(message);

        return ret;
    }

    message_free(message);

    answer = MESSAGE_EXTRACT(struct resp_ioctl, response);

    if (answer->ret < 0)
    {
        message_free(response);

        return answer->ret;
    }

    if (argp && answer->modify_argp)
        *argp = answer->argp;

    message_free(response);

    return 0;
}

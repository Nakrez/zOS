#include <kernel/errno.h>
#include <kernel/thread.h>
#include <kernel/panic.h>

#include <kernel/scheduler/event.h>

#include <kernel/vfs/vfs.h>
#include <kernel/vfs/vfile.h>
#include <kernel/vfs/path_tree.h>
#include <kernel/vfs/vops.h>
#include <kernel/vfs/vdevice.h>
#include <kernel/vfs/vchannel.h>
#include <kernel/vfs/message.h>

static int vfs_send_recv(struct vchannel *channel, struct message *message,
                         struct message **response)
{
    int res;
    int req_id;

    if ((res = channel_send_request(channel, message)) < 0)
        return res;

    /* Block the current thread until we have our answer */
    thread_block(thread_current(), SCHED_EV_RESP, message->mid, NULL);

    /* Save the request id */
    req_id = message->mid;

    /* Get our response, thanks to the request id saved */
    if ((res = channel_recv_response(channel, req_id, response)) < 0)
        return res;

    return 0;
}

static int check_fd(struct process *process, int fd, int op,
                    struct vdevice **dev)
{
    struct vdevice *device;

    if (!process->files[fd].used)
        return -EBADF;

    /* if (!(device = device_get(process->files[fd].vnode->dev))) */
    /*     return -ENODEV; */

    if (!(device->ops & op))
        return -EINVAL;

    *dev = device;

    return 0;
}

int vfs_write(int fd, const void *buf, size_t count)
{
    int res;
    struct message *message = NULL;
    struct message *mresponse = NULL;
    struct process *process = thread_current()->parent;
    struct process *pdevice;
    struct vdevice *device;
    struct rdwr_msg *request;

    if ((res = check_fd(process, fd, VFS_OPS_WRITE, &device)) < 0)
        return res;

    if (!(message = message_alloc(sizeof (struct rdwr_msg))))
        return -ENOMEM;

    request = (void *)(message + 1);

    /* This index has been returned by open and only matters to filesystem */
    /* request->index = process->files[fd].vnode->index; */
    request->size = count;
    request->off = process->files[fd].offset;

    pdevice = process_get(device->pid);

    request->data = (void *)as_map(pdevice->as, 0, 0, count,
                                   AS_MAP_USER | AS_MAP_WRITE);

    if (!request->data)
    {
        message_free(message);

        return -ENOMEM;
    }

    res = as_copy(process->as, pdevice->as, buf, request->data, count);

    if (res < 0)
        goto end;

    message->mid = (message->mid & ~0xFF) | VFS_OPS_WRITE;

    if ((res = vfs_send_recv(device->channel, message, &mresponse)) < 0)
        goto end;

    struct msg_response *response = (void *)(mresponse + 1);

    res = response->ret;

    if (response->ret > 0)
        process->files[fd].offset += res;

end:
    as_unmap(pdevice->as, (vaddr_t)request->data, AS_UNMAP_RELEASE);
    message_free(message);
    message_free(mresponse);

    return res;
}

int vfs_close(int fd)
{
    int res;
    struct process *process = thread_current()->parent;
    struct vdevice *device;
    struct close_msg *request;
    struct message *message = NULL;
    struct message *mresponse = NULL;
    struct msg_response *response;

    if ((res = check_fd(process, fd, VFS_OPS_CLOSE, &device)) < 0)
        return res;

    if (!(message = message_alloc(sizeof (struct close_msg))))
        return -ENOMEM;

    request = (void *)(message + 1);

    /* request->index = process->files[fd].vnode->index; */

    message->mid = (message->mid & ~0xFF) | VFS_OPS_CLOSE;

    if ((res = vfs_send_recv(device->channel, message, &mresponse)) < 0)
        goto error;

    response = (void *)(mresponse + 1);

    if (response->ret < 0)
    {
        res = response->ret;

        goto error;
    }

    message_free(message);
    message_free(mresponse);

    process_free_fd(process, fd);

    return 0;
error:
    message_free(message);
    message_free(mresponse);

    return res;
}

int vfs_lseek(int fd, int offset, int whence)
{
    struct process *process = thread_current()->parent;

    if (!(whence & VFS_SEEK_SET) && !(whence && VFS_SEEK_CUR) &&
        !(whence && VFS_SEEK_END))
        return -EINVAL;

    if (fd < 0 || fd > PROCESS_MAX_OPEN_FD)
        return -EINVAL;

    if (!process->files[fd].used)
        return -EINVAL;

    if (whence & VFS_SEEK_SET)
    {
        if (offset < 0)
            return -EINVAL;

        process->files[fd].offset = offset;
    }
    else if (whence & VFS_SEEK_CUR)
    {
        size_t old_off = process->files[fd].offset;

        process->files[fd].offset += offset;

        if (offset < 0 && process->files[fd].offset > old_off)
        {
            process->files[fd].offset = old_off;

            return -EINVAL;
        }

        if (offset > 0 && process->files[fd].offset < old_off)
        {
            process->files[fd].offset = old_off;

            return -EINVAL;
        }
    }
    else
        kernel_panic("VFS_SEEK_END not implemented yet");

    return 0;
}

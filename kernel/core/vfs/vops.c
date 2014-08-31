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

static int vfs_open_extract_channel(struct vtree_node *node,
                                    struct vchannel **channel)
{
    struct vdevice *device;

    if (!(node->vnode->type & VFS_TYPE_DEVICE))
    {
        kernel_panic("vfs_open: Need implem of open() for non device files");

        return 0;
    }
    else
    {
        if (!(device = device_get(node->vnode->dev)))
            return -ENODEV;

        *channel = device->channel;
    }

    return 0;
}

int vfs_open(const char *pathname, int flags, int mode)
{
    (void) flags;
    (void) mode;

    int res;
    int req_id;
    int fd;
    struct vtree_node *node;
    const char *remaining;
    struct message *message;
    struct vchannel *channel;
    struct process *process = thread_current()->parent;

    /* Identify the node in the vtree */
    if ((res = vtree_lookup(pathname, &remaining, &node)) < 0)
        return res;

    if (*remaining)
        kernel_panic("vfs_open: Need implem of non populated path");

    /* TODO: Check access rights */

    /* Cannot open a directory */
    if (node->vnode->type & VFS_TYPE_DIR)
        return -EISDIR;

    /* Get the communication channel for our request */
    if ((res = vfs_open_extract_channel(node, &channel)) < 0)
        return res;

    /* Allocate a new message for the request */
    if (!(message = message_alloc(sizeof (struct open_msg))))
        return -ENOMEM;

    /* Get a file descriptor */
    if ((fd = process_new_fd(process)) < 0)
    {
        message_free(message);

        return fd;
    }

    /* Message id contains the request type as well */
    message->mid = (message->mid & ~0xFF) | VFS_OPS_OPEN;

    /*
     * TODO: Format request, only some drivers can ignore the content of the
     * request some other need it
     */

    /* Send the request through the channel */
    if ((res = channel_send_request(channel, message)) < 0)
    {
        message_free(message);
        process_free_fd(process, fd);

        return res;
    }

    /* Block the current thread until we have our answer */
    thread_block(thread_current(), SCHED_EV_RESP, message->mid);

    /* Save the request id */
    req_id = message->mid;

    /* Delete the request, not needed anymore */
    message_free(message);
    message = NULL;

    /* Get our response, thanks to the request id saved */
    if ((res = channel_recv_response(channel, req_id, &message)) < 0)
    {
        process_free_fd(process, fd);

        return res;
    }

    /* Process the response */
    struct msg_response *response = (void *)(message + 1);

    if (response->ret)
    {
        process_free_fd(process, fd);

        return response->ret;
    }

    /* TODO: Set the rest */
    process->files[fd].offset = 0;
    process->files[fd].vnode = node->vnode;

    ++node->vnode->ref_count;

    /* Delete the response message */
    message_free(message);

    return fd;
}

int vfs_read(int fd, void *buf, size_t count)
{
    int res;
    struct process *process = thread_current()->parent;
    struct process *pdevice;
    struct vdevice *device;
    struct message *message;
    struct message *mresponse;
    struct read_msg *request;

    if (!process->files[fd].used)
        return -EBADF;

    if (!(device = device_get(process->files[fd].vnode->dev)))
        return -ENODEV;

    if (!(device->ops & VFS_OPS_READ))
        return -EINVAL;

    if (!(message = message_alloc(sizeof (struct read_msg))))
        return -ENOMEM;

    request = (void *)(message + 1);

    /* This index has been returned by open and only matters to filesystem */
    request->index = process->files[fd].vnode->index;
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

    message->mid = (message->mid & ~0xFF) | VFS_OPS_READ;

    /* Send the request through the channel */
    if ((res = channel_send_request(device->channel, message)) < 0)
    {
        message_free(message);
        as_unmap(pdevice->as, (vaddr_t)request->data, AS_UNMAP_RELEASE);

        return res;
    }

    /* Block the current thread until we have our answer */
    thread_block(thread_current(), SCHED_EV_RESP, message->mid);

    if ((res = channel_recv_response(device->channel, message->mid,
                                     &mresponse)) < 0)
    {
        message_free(message);
        as_unmap(pdevice->as, (vaddr_t)request->data, AS_UNMAP_RELEASE);

        return res;
    }

    struct msg_response *response = (void *)(mresponse + 1);

    if (response->ret < 0)
    {
        res = response->ret;

        message_free(message);
        message_free(mresponse);

        as_unmap(pdevice->as, (vaddr_t)request->data, AS_UNMAP_RELEASE);

        return res;
    }

    res = as_copy(pdevice->as, process->as, request->data, buf, count);

    if (res == 0)
        res = response->ret;

    as_unmap(pdevice->as, (vaddr_t)request->data, AS_UNMAP_RELEASE);
    message_free(message);
    message_free(mresponse);

    return res;
}

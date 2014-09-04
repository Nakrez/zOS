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

static int vfs_send_recv(struct vchannel *channel, struct message *message,
                         struct message **response)
{
    int res;
    int req_id;

    if ((res = channel_send_request(channel, message)) < 0)
        return res;

    /* Block the current thread until we have our answer */
    thread_block(thread_current(), SCHED_EV_RESP, message->mid);

    /* Save the request id */
    req_id = message->mid;

    /* Get our response, thanks to the request id saved */
    if ((res = channel_recv_response(channel, req_id, response)) < 0)
        return res;

    return 0;
}

int vfs_open(const char *pathname, int flags, int mode)
{
    (void) flags;
    (void) mode;

    int res;
    int fd;
    struct vtree_node *node;
    const char *remaining;
    struct message *message = NULL;
    struct message *response = NULL;
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

    if ((res = vfs_send_recv(channel, message, &response)) < 0)
    {
        message_free(message);
        process_free_fd(process, fd);

        return res;
    }

    message_free(message);

    /* Process the response */
    struct msg_response *mresponse = (void *)(response + 1);

    if (mresponse->ret)
    {
        process_free_fd(process, fd);

        res = mresponse->ret;

        message_free(response);

        return res;
    }

    /* TODO: Set the rest */
    process->files[fd].offset = 0;
    process->files[fd].vnode = node->vnode;

    ++node->vnode->ref_count;

    /* Delete the response message */
    message_free(response);

    return fd;
}

static int check_fd(struct process *process, int fd, int op,
                    struct vdevice **dev)
{
    struct vdevice *device;

    if (!process->files[fd].used)
        return -EBADF;

    if (!(device = device_get(process->files[fd].vnode->dev)))
        return -ENODEV;

    if (!(device->ops & op))
        return -EINVAL;

    *dev = device;

    return 0;
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

    if ((res = check_fd(process, fd, VFS_OPS_READ, &device)) < 0)
        return res;

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

    if ((res = vfs_send_recv(device->channel, message, &mresponse)) < 0)
        goto end;

    struct msg_response *response = (void *)(mresponse + 1);

    if (response->ret < 0)
    {
        res = response->ret;

        goto end;
    }

    res = as_copy(pdevice->as, process->as, request->data, buf, count);

    if (res == 0)
    {
        res = response->ret;
        process->files[fd].offset += res;
    }

end:
    as_unmap(pdevice->as, (vaddr_t)request->data, AS_UNMAP_RELEASE);
    message_free(message);
    message_free(mresponse);

    return res;
}

/*
 * zOS
 * Copyright (C) 2014 - 2015 Baptiste Covolato
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with zOS.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file    kernel/core/vfs/vdevice.c
 * \brief   Implementation of virtual devices management
 *
 * \author  Baptiste Covolato
 */

#include <string.h>

#include <kernel/errno.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/proc/thread.h>

#include <kernel/vfs/vfs.h>
#include <kernel/vfs/vdevice.h>

#include <arch/spinlock.h>

static struct vdevice devices[VFS_MAX_DEVICE];
static spinlock_t device_lock = SPINLOCK_INIT;

static dev_t device_create(pid_t pid, const char *name, vop_t ops,
                           struct vdevice **device)
{
    int found = 0;

    if (!(ops & VFS_OPS_OPEN) || !(ops & VFS_OPS_CLOSE))
        return -EINVAL;

    *device = NULL;

    spinlock_lock(&device_lock);

    for (int i = 0; i < VFS_MAX_DEVICE; ++i)
    {
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

dev_t vfs_device_create(const char *name, pid_t pid, int perm, int ops)
{
    int res;
    struct vdevice *device = NULL;
    char *node_path;

    res = device_create(pid, name, ops, &device);

    if (res < 0)
        return res;

    /*
     * If mknod fails we don't really care because the device exists node the
     * node in the FS, but it can be created later
     */

    /* 5 = strlen("/dev/") */
    if (!(node_path = kmalloc(5 + strlen(name) + 1)))
        return device->id;

    strcpy(node_path, "/dev/");

    strcat(node_path, name);

    vfs_mknod(thread_current(), node_path, perm, device->id);

    kfree(node_path);

    return device->id;
}

struct vdevice *device_get(dev_t dev)
{
    if (dev < 0 || dev >= VFS_MAX_DEVICE)
        return NULL;

    if (!devices[dev].active)
        return NULL;

    return &devices[dev];
}

int device_exists(const char *name)
{
    for (int i = 0; i < VFS_MAX_DEVICE; ++i)
    {
        if (devices[i].active && !strcmp(devices[i].name, name))
            return 1;
    }

    return 0;
}

int device_recv_request(dev_t dev, char *buf, size_t size)
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

int device_send_response(dev_t dev, uint32_t req_id, char *buf, size_t size)
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

int device_open(dev_t dev, ino_t inode, pid_t pid, uid_t uid, gid_t gid,
                int flags, mode_t mode)
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
    request->pid = pid;

    message->mid = (message->mid & ~0xFF) | VFS_OPEN;

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

int device_read_write(struct process *process, dev_t dev, struct req_rdwr *req,
                      char *buf, int op)
{
    int res;
    struct vdevice *device;
    struct process *pdevice;
    struct message *message = NULL;
    struct req_rdwr *request;
    struct message *response = NULL;
    struct resp_rdwr *answer;

    if (!(device = device_get(dev)))
        return -ENODEV;

    if (!(device->ops & op))
        return -ENOSYS;

    if (!(message = message_alloc(sizeof (struct req_rdwr))))
        return -ENOMEM;

    request = MESSAGE_EXTRACT(struct req_rdwr, message);

    request->inode = req->inode;
    request->size = req->size;
    request->off = req->off;

    pdevice = process_get(device->pid);

    request->data = (void *)as_map(pdevice->as, 0, 0, req->size,
                                   AS_MAP_USER | AS_MAP_WRITE);

    if (!request->data)
    {
        message_free(message);

        return -ENOMEM;
    }

    if (op == VFS_WRITE)
    {
        res = as_copy(process->as, pdevice->as, buf, request->data,
                      request->size);

        if (res < 0)
            goto end;
    }

    message->mid = (message->mid & ~0xFF) | op;

    if ((res = channel_send_recv(device->channel, message, &response)) < 0)
        goto end;

    answer = MESSAGE_EXTRACT(struct resp_rdwr, response);

    if (answer->ret < 0)
    {
        res = answer->ret;

        goto end;
    }

    if (op == VFS_READ)
        res = as_copy(pdevice->as, process->as, request->data, buf,
                      answer->size);

    if (res == 0)
    {
        res = answer->size;

        req->off += answer->size;
    }

end:
    as_unmap(pdevice->as, (vaddr_t)request->data, AS_UNMAP_RELEASE);
    message_free(message);
    message_free(response);

    return res;
}

int device_close(dev_t dev, ino_t inode)
{
    int res;
    struct vdevice *device;
    struct message *message;
    struct message *response;
    struct req_close *request;
    struct resp_close *answer;

    if (!(device = device_get(dev)))
        return -ENODEV;

    if (!(message = message_alloc(sizeof (struct req_close))))
        return -ENOMEM;

    request = MESSAGE_EXTRACT(struct req_close, message);

    request->inode = inode;

    message->mid = (message->mid & ~0xFF) | VFS_CLOSE;

    if ((res = channel_send_recv(device->channel, message, &response)) < 0)
    {
        message_free(message);

        return res;
    }

    answer = MESSAGE_EXTRACT(struct resp_close, message);

    res = answer->ret;

    message_free(message);
    message_free(response);

    return res;
}

int device_destroy(pid_t pid, dev_t dev)
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

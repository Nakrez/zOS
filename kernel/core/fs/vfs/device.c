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
 * \file    kernel/core/vfs/device.c
 * \brief   Implementation of virtual devices management
 *
 * \author  Baptiste Covolato
 */

#include <string.h>

#include <kernel/errno.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/proc/thread.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/vfs/device.h>
#include <kernel/fs/vfs/message.h>

#include <arch/spinlock.h>

static struct device devices[VFS_MAX_DEVICE];
static spinlock_t device_lock = SPINLOCK_INIT;

/**
 *  \brief  Find a free device id and verify that a device named \a name
 *          does not exist
 *
 *  \param  name    The name of the device you want to create
 *
 *  \return  The device if everything went well, NULL otherwise
 */
static dev_t find_free_device(const char *name, struct device **dev)
{
    dev_t id = -1;

    for (int i = 0; i < VFS_MAX_DEVICE; ++i) {
        if (id == -1 && !devices[i].active)
            id = i;

        if (devices[i].active && !strcmp(devices[i].name, name))
            return -EEXIST;
    }

    if (id == -1)
        return -EBUSY;

    *dev = &devices[id];

    return id;
}

static dev_t device_create(pid_t pid, const char *name, vop_t ops,
                           struct file_operation *f_ops,
                           struct device **device)
{
    dev_t dev_id;
    struct device *new_dev;

    if (!(ops & VFS_OPS_OPEN) || !(ops & VFS_OPS_CLOSE))
        return -EINVAL;

    spinlock_lock(&device_lock);

    dev_id = find_free_device(name, &new_dev);
    if (dev_id < 0) {
        spinlock_unlock(&device_lock);
        return dev_id;
    }

    new_dev->active = 1;
    strncpy(new_dev->name, name, VFS_DEV_MAX_NAMEL);

    spinlock_unlock(&device_lock);

    new_dev->channel = vchannel_create();
    if (!new_dev->channel) {
        new_dev->active = 0;
        return -ENOMEM;
    }

    new_dev->id = dev_id;
    new_dev->pid = pid;
    new_dev->ops = ops;
    new_dev->f_ops = f_ops;

    if (device)
        *device = new_dev;

    return dev_id;
}

dev_t vfs_device_create(const char *name, pid_t pid, int perm, int ops,
                        struct file_operation *f_ops)
{
    (void) perm;

    return device_create(pid, name, ops, f_ops, NULL);
}

struct device *device_get(dev_t dev)
{
    if (dev < 0 || dev >= VFS_MAX_DEVICE)
        return NULL;

    if (!devices[dev].active)
        return NULL;

    return &devices[dev];
}

dev_t device_get_from_name(const char *name)
{
    for (int i = 0; i < VFS_MAX_DEVICE; ++i) {
        if (!devices[i].active)
            continue;

        if (!strcmp(devices[i].name, name))
            return devices[i].id;
    }

    return -1;
}

int device_exists(const char *name)
{
    for (int i = 0; i < VFS_MAX_DEVICE; ++i) {
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

    return vchannel_recv_request(devices[dev].channel, buf, size);
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

    res = vchannel_send_response(devices[dev].channel, req_id, message);

    if (res < 0)
        message_free(message);

    return res;
}

int device_open(dev_t dev, ino_t inode, pid_t pid, uid_t uid, gid_t gid,
                int flags, mode_t mode)
{
    int ret = 0;
    struct device *device;
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

    ret = vchannel_send_recv(device->channel, message, &response);
    if (ret < 0) {
        message_free(message);
        return ret;
    }

    message_free(message);

    answer = MESSAGE_EXTRACT(struct resp_open, response);

    if (answer->ret < 0) {
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
    struct device *device;
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

    if (!request->data) {
        message_free(message);
        return -ENOMEM;
    }

    if (op == VFS_WRITE) {
        res = as_copy(process->as, pdevice->as, buf, request->data,
                      request->size);
        if (res < 0)
            goto end;
    }

    message->mid = (message->mid & ~0xFF) | op;

    res = vchannel_send_recv(device->channel, message, &response);
    if (res < 0)
        goto end;

    answer = MESSAGE_EXTRACT(struct resp_rdwr, response);

    if (answer->ret < 0) {
        res = answer->ret;
        goto end;
    }

    if (op == VFS_READ)
        res = as_copy(pdevice->as, process->as, request->data, buf,
                      answer->size);

    if (res == 0) {
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
    struct device *device;
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

    res = vchannel_send_recv(device->channel, message, &response);
    if (res < 0) {
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

    spinlock_unlock(&device_lock);

    return 0;
}

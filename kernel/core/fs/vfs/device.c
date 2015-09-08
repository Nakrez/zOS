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
                           struct file_operation *f_ops, void *private,
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

    new_dev->id = dev_id;
    new_dev->pid = pid;
    new_dev->ops = ops;
    new_dev->f_ops = f_ops;
    new_dev->private = private;

    if (device)
        *device = new_dev;

    return dev_id;
}

dev_t vfs_device_create(const char *name, pid_t pid, int perm, int ops,
                        struct file_operation *f_ops, void *private)
{
    (void) perm;

    return device_create(pid, name, ops, f_ops, private, NULL);
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

    return -ENODEV;
}

struct device *device_get_from_index(int index)
{
    if (index < 0 || index > VFS_MAX_DEVICE)
        return NULL;

    spinlock_lock(&device_lock);

    for (int i = 0; i < VFS_MAX_DEVICE; ++i) {
        if (devices[i].active) {
            if (!index) {
                spinlock_unlock(&device_lock);
                return &devices[i];
            }

            --index;
        }
    }

    spinlock_unlock(&device_lock);

    return NULL;
}

int device_exists(const char *name)
{
    for (int i = 0; i < VFS_MAX_DEVICE; ++i) {
        if (devices[i].active && !strcmp(devices[i].name, name))
            return 1;
    }

    return 0;
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

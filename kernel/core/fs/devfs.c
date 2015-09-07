/*
 * zOS
 * Copyright (C) 2015 Baptiste Covolato
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
 * \file    kernel/core/fs/devfs.h
 * \brief   Definition of function and structure related to the devfs file
 *          system
 *
 * \author  Baptiste Covolato
 */

#include <kernel/zos.h>
#include <kernel/console.h>
#include <kernel/errno.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/devfs.h>

# define FS_NAME "devfs"

static int devfs_create(struct fs_instance *fi, const char __unused *device,
                        const char __unused *mount_pt)
{
    struct devfs *devfs = fi->parent->private;

    spinlock_lock(&devfs->lock);

    /* Devfs can be mounted only once */
    if (devfs->mounted) {
        spinlock_unlock(&devfs->lock);
        return -EBUSY;
    }

    devfs->mounted = 1;
    spinlock_unlock(&devfs->lock);

    return 0;
}

static struct file_operation devfs_f_ops = {
    .open = NULL,
};

static struct fs_operation devfs_ops = {
    .lookup = NULL,
};

static struct fs_super_operation devfs_sup_ops = {
    .create = devfs_create,
};

int devfs_initialize(void)
{
    struct devfs *devfs;

    devfs = kmalloc(sizeof (struct devfs));
    if (!devfs)
        return -ENOMEM;

    devfs->mounted = 0;
    spinlock_init(&devfs->lock);

    return fs_register(FS_NAME, 0, &devfs_sup_ops, &devfs_ops, &devfs_f_ops,
                       devfs);
}

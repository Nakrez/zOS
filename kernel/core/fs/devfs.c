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

#include <string.h>

#include <kernel/zos.h>
#include <kernel/errno.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/devfs.h>

#include <kernel/fs/vfs/device.h>
#include <kernel/fs/vfs/message.h>

# define FS_NAME "devfs"

# define DEVFS_INODE_ROOT (VFS_MAX_DEVICE + 1)

static int devfs_open(struct file __unused *file, ino_t inode,
                      pid_t __unused pid, uid_t __unused uid,
                      gid_t __unused gid, int __unused flags,
                      mode_t __unused mode)
{
    if (inode != DEVFS_INODE_ROOT)
        return -EINVAL;

    return DEVFS_INODE_ROOT;
}

static int devfs_close(struct file __unused *file, ino_t __unused inode)
{
    return 0;
}

static int devfs_lookup(struct mount_entry __unused *root, const char *path,
                        uid_t __unused uid, gid_t __unused gid,
                        struct resp_lookup *resp)
{
    size_t path_len;

    resp->processed = 0;

    if (*path == '/') {
        ++path;
        ++resp->processed;
    }

    /* Root node */
    if (!strcmp(path, "") || !strcmp(path, "/")) {
        resp->inode.dev = -1;
        resp->inode.inode = DEVFS_INODE_ROOT;
        resp->ret = RES_OK;

        return 0;
    }

    /* Devfs only contains for now one level with only devices */
    path_len = strlen(path);
    if (path_len > VFS_DEV_MAX_NAMEL)
        return -ENOENT;

    resp->inode.dev = device_get_from_name(path);
    if (resp->inode.dev < 0)
        return -ENOENT;

    resp->processed += path_len;
    resp->inode.inode = resp->inode.dev;
    resp->ret = RES_OK;

    return 0;
}

static int devfs_stat(struct mount_entry __unused *mount, uid_t __unused uid,
                      gid_t __unused gid, ino_t inode, struct stat *stat)
{
    if (inode == DEVFS_INODE_ROOT) {
        stat->st_dev = -1;
        stat->st_ino = DEVFS_INODE_ROOT;
        stat->st_mode = VFS_FTYPE_DIR;
    } else {
        struct device *device;

        device = device_get(inode);
        if (!device)
            return -EINVAL;

        stat->st_dev = device->id;
        stat->st_ino = device->id;
        stat->st_mode = VFS_FTYPE_DEV;
    }

    /* XXX: Set correct values */
    stat->st_nlink = 0;
    stat->st_uid = 0;
    stat->st_gid = 0;
    stat->st_rdev = 0;
    stat->st_size = 0;
    stat->st_blksize = 0;
    stat->st_blocks = 0;
    stat->st_atime = 0;
    stat->st_mtime = 0;
    stat->st_ctime = 0;

    return 0;
}

static int devfs_getdirent(struct mount_entry __unused *mount, ino_t inode,
                           struct dirent *dirent, int index)
{
    struct device *device;

    if (inode != DEVFS_INODE_ROOT)
        return -EINVAL;

    device = device_get_from_index(index);
    if (!device)
        return 0;

    dirent->d_ino = device->id;

    strncpy(dirent->d_name, device->name, VFS_DEV_MAX_NAMEL);

    return 1;
}

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
    .open = devfs_open,
    .close = devfs_close,
};

static struct fs_operation devfs_ops = {
    .lookup = devfs_lookup,
    .stat = devfs_stat,
    .getdirent = devfs_getdirent,
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

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
 * \file    kernel/core/fs/vfs/fs.h
 * \brief   Implementation of file system manipulation
 *
 * \author  Baptiste Covolato
 */

#include <string.h>

#include <kernel/errno.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/fs/vfs.h>

#include <arch/spinlock.h>

static spinlock_t fs_lock;
static struct klist fs_list;

int fs_initialize(void)
{
    spinlock_init(&fs_lock);

    klist_head_init(&fs_list);

    return 0;
}

static struct fs *fs_from_name_nolock(const char *name)
{
    struct fs *fs;

    klist_for_each_elem(&fs_list, fs, list) {
        if (!strncmp(fs->name, name, VFS_FS_MAX_NAMEL))
            return fs;
    }

    return NULL;
}

struct fs *fs_from_name(const char *name)
{
    struct fs *fs;

    spinlock_lock(&fs_lock);

    fs = fs_from_name_nolock(name);

    spinlock_unlock(&fs_lock);

    return fs;
}

int fs_register(const char *name, pid_t pid,
                struct fs_super_operation *fs_sup_ops,
                struct fs_operation *fs_ops, struct file_operation *f_ops,
                void *private)
{
    struct fs *fs;

    /* We need a way to instantiate a fs */
    if (!fs_sup_ops->create)
        return -EINVAL;

    fs = kmalloc(sizeof (struct fs));
    if (!fs)
        return -ENOMEM;

    strncpy(fs->name, name, VFS_FS_MAX_NAMEL);
    fs->pid = pid;
    fs->fs_super_ops = fs_sup_ops;
    fs->fs_ops = fs_ops;
    fs->f_ops = f_ops;
    fs->private = private;
    klist_head_init(&fs->instances);
    spinlock_init(&fs->lock);

    spinlock_lock(&fs_lock);

    if (fs_from_name_nolock(name)) {
        kfree(fs);
        spinlock_unlock(&fs_lock);
        return -EEXIST;
    }

    klist_add(&fs_list, &fs->list);
    spinlock_unlock(&fs_lock);

    return 0;
}

int fs_unregister(const char *name, pid_t pid)
{
    struct fs *fs;

    spinlock_lock(&fs_lock);

    fs = fs_from_name_nolock(name);
    if (!fs) {
        spinlock_unlock(&fs_lock);
        return -ENOENT;
    }

    if (fs->pid != pid) {
        spinlock_unlock(&fs_lock);
        return -EPERM;
    }

    klist_del(&fs->list);

    spinlock_unlock(&fs_lock);

    kfree(fs);

    return 0;
}

int fs_new_instance(const char *name, const char *device, const char *mount_pt,
                    struct fs_instance **fi)
{
    int ret;
    struct fs *fs;
    struct fs_instance *instance;

    fs = fs_from_name(name);
    if (!fs)
        return -ENOENT;

    instance = kmalloc(sizeof (struct fs_instance));
    if (!instance)
        return -ENOMEM;

    instance->parent = fs;
    instance->private = NULL;

    ret = instance->parent->fs_super_ops->create(instance, device, mount_pt);
    if (ret < 0) {
        kfree(instance);
        return ret;
    }

    spinlock_lock(&fs->lock);
    klist_add(&fs->instances, &instance->list);
    spinlock_unlock(&fs->lock);

    if (fi)
        *fi = instance;

    return 0;
}

void fs_del_instance(struct fs_instance *instance)
{
    spinlock_lock(&instance->parent->lock);

    klist_del(&instance->list);

    spinlock_unlock(&instance->parent->lock);

    kfree(instance);
}

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
 * \file    kernel/core/vfs/vfs.c
 * \brief   Implementation of virtual filesystem initialization
 *
 * \author  Baptiste Covolato
 */

#include <string.h>

#include <kernel/config.h>
#include <kernel/errno.h>
#include <kernel/console.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/proc/thread.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/channel.h>

#include <kernel/fs/vfs/vops.h>
#include <kernel/fs/vfs/mount.h>

#ifdef CONFIG_DEVFS
# include <kernel/fs/devfs.h>
#endif /* !CONFIG_DEVFS */

int vfs_initialize(void)
{
    int ret;

    ret = channel_initialize();
    if (ret < 0)
        return ret;

    ret = fs_initialize();
    if (ret < 0)
        return ret;

#ifdef CONFIG_DEVFS
    ret = devfs_initialize();
    if (ret < 0)
        return ret;
#endif /* !CONFIG_DEVFS */

    return 0;
}

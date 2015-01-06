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

#include <kernel/errno.h>
#include <kernel/console.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/proc/thread.h>

#include <kernel/vfs/vfs.h>
#include <kernel/vfs/tmpfs.h>
#include <kernel/vfs/vops.h>
#include <kernel/vfs/vdevice.h>

int vfs_initialize(void)
{
    if (vfs_mount(NULL, TMPFS_DEV_ID, "/") < 0)
    {
        console_message(T_ERR, "Fail to mount tmpfs on /");

        return -1;
    }

    console_message(T_OK, "Tmpfs mounted on /");

    if (vfs_mkdir(NULL, "/dev", VFS_PERM_USER_RWX | VFS_PERM_GROUP_RX |
                                VFS_PERM_OTHER_RW) < 0)
    {
        console_message(T_ERR, "Fail to create /dev");

        return -1;
    }

    if (vfs_mount(NULL, TMPFS_DEV_ID, "/dev") < 0)
    {
        console_message(T_ERR, "Fail to mount tmpfs on /dev");

        return -1;
    }

    console_message(T_OK, "Tmpfs mounted on /dev");

    return 0;
}

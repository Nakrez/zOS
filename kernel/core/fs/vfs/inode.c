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
 * \file    kernel/core/fs/vfs/inode.c
 * \brief   Implementation of functions related to inode management
 *
 * \author  Baptiste Covolato
 */

#include <string.h>

#include <kernel/mem/kmalloc.h>

#include <kernel/fs/vfs.h>

struct inode *inode_new(mode_t mode)
{
    struct inode *inode;

    inode = kmalloc(sizeof (struct inode));
    if (!inode)
        return NULL;

    memset(inode, 0, sizeof (struct inode));

    inode->ref = 1;
    inode->mode = mode;

    return inode;
}

void inode_del(struct inode *inode)
{
    --inode->ref;

    if (!inode->ref)
        kfree(inode);
}

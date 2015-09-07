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
 * \file    include/kernel/fs/devfs.h
 * \brief   Definition of function and structure related to the devfs file
 *          system
 *
 * \author  Baptiste Covolato
 */

#ifndef FS_DEVFS_H
# define FS_DEVFS_H

#include <arch/spinlock.h>

/**
 *  \brief  General information about the devfs
 */
struct devfs {
    /**
     *  \brief  The devfs can be mounted only once
     */
    int mounted;

    /**
     *  \brief  Avoid races on the mounted field
     */
    spinlock_t lock;
};

/**
 *  \brief  Initialize and register the devfs file system
 *
 *  \return 0: Everything went well
 */
int devfs_initialize(void);

#endif /* !FS_DEVFS_H */

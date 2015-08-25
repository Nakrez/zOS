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
 * \file    include/kernel/vfs/device.h
 * \brief   Function prototypes and structures related to virtual devices
 *          management.
 *
 * \author  Baptiste Covolato
 */

#ifndef FS_VFS_DEVICE_H
# define FS_VFS_DEVICE_H

# include <kernel/types.h>
# include <kernel/zos.h>

/**
 * \brief   The maximum number of devices that can be registered in the system
 */
# define VFS_MAX_DEVICE 255
# define VFS_DEV_MAX_NAMEL 15

/**
 * \brief   The internal representation of a virtual device
 */
struct device {
    /**
     * \brief   The name of the device
     */
    char name[VFS_DEV_MAX_NAMEL];

    /**
     * \brief   Tells if this device is used and active
     */
    int active;

    /**
     * \brief   The id of the device
     */
    dev_t id;

    /**
     * \brief   The process that is defines this devices
     */
    int pid;

    /**
     * \brief   The capability of the devices (operation that it supports)
     */
    vop_t ops;

    /**
     *  \brief  The file operation associated with the device
     */
    struct file_operation *f_ops;

    /**
     *  \brief  Private data for the driver
     */
    void *private;
};

/**
 * \brief   Create a new device
 *
 * \param   name    The name of the device
 * \param   pid     The pid of the creating process
 * \param   perm    Unix permission for the created node access in /dev
 * \param   ops     Operations supported by the device
 * \param   f_ops   File operation associated with the device
 *
 * \return  The device id the device was created
 * \return  -EINVAL: Invalid supported operations
 * \return  -ENOMEM: Cannot allocate memory
 * \return  -EEXIST: Device already exists
 */
dev_t vfs_device_create(const char *name, pid_t pid, int perm, int ops,
                        struct file_operation *f_ops, void *private);

/**
 * \brief   Get device structure from device id
 *
 * \param   dev The device id
 *
 * \return  The device corresponding to \a dev, if it exists
 * \return  NULL: device does not exist or \a dev is invalid
 */
struct device *device_get(dev_t dev);

/**
 *  \brief  Get device structure from name
 *
 *  \param  name    The name of the device you want to get
 *
 * \return  The device corresponding to \a dev, if it exists
 * \return  -1: device does not exist or \a dev is invalid
 */
dev_t device_get_from_name(const char *name);

/**
 * \brief   Check if a device exists
 *
 * \param   name    The name of the device
 *
 * \return  1: The device exists
 * \return  0: The device does not exists
 */
int device_exists(const char *name);

/**
 * \brief   Destroy a device
 *
 * \param   pid The pid of the requesting process
 * \param   dev The device id of the device you want to destroy
 *
 * \return  0: Success
 * \return  -EINVAL: Wrong value of \a dev or \a pid is not the owner of the
 *          device
 * \return  -ENODEV: Device id is not an active device
 */
int device_destroy(pid_t pid, dev_t dev);

#endif /* !DEVICE_H */

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
 * \file    include/kernel/vfs/vfs.h
 * \brief   Various virtual filesystem constants and prototype of
 *          initialization function
 *
 * \author  Baptiste Covolato
 */

#ifndef VFS_H
# define VFS_H

/**
 * \brief   The maximum size of a filename
 */
# define VFS_MAX_FILENAME_SIZE 255

/**
 * \def VFS_OPEN
 * VFS open message identifier
 *
 * \def VFS_READ
 * VFS read message identifier
 *
 * \def VFS_WRITE
 * VFS write message identifier
 *
 * \def VFS_CLOSE
 * VFS close message identifier
 *
 * \def VFS_UMOUNT
 * VFS umount message identifier
 *
 * \def VFS_MOUNT
 * VFS mount message identifier
 *
 * \def VFS_LOOKUP
 * VFS lookup message identifier
 *
 * \def VFS_MKDIR
 * VFS mkdir message identifier
 *
 * \def VFS_MKNOD
 * VFS mknod message identifier
 *
 * \def VFS_STAT
 * VFS stat message identifier
 *
 * \def VFS_IOCTL
 * VFS ioctl message identifier
 *
 * \def VFS_GETDIRENT
 * VFS getdirent message identifier
 */
# define VFS_OPEN 1
# define VFS_READ 2
# define VFS_WRITE 3
# define VFS_CLOSE 4
# define VFS_UMOUNT 5
# define VFS_MOUNT 6
# define VFS_LOOKUP 7
# define VFS_MKDIR 8
# define VFS_MKNOD 9
# define VFS_STAT 10
# define VFS_IOCTL 11
# define VFS_GETDIRENT 12

/**
 * \def VFS_OPS_OPEN
 * VFS open capability
 *
 * \def VFS_OPS_READ
 * VFS read capability
 *
 * \def VFS_OPS_WRITE
 * VFS write capability
 *
 * \def VFS_OPS_CLOSE
 * VFS close capability
 *
 * \def VFS_OPS_UMOUNT
 * VFS umount capability
 *
 * \def VFS_OPS_MOUNT
 * VFS mount capability
 *
 * \def VFS_OPS_LOOKUP
 * VFS lookup capability
 *
 * \def VFS_OPS_MKDIR
 * VFS mkdir capability
 *
 * \def VFS_OPS_MKNOD
 * VFS mknod capability
 *
 * \def VFS_OPS_STAT
 * VFS stat capability
 *
 * \def VFS_OPS_IOCTL
 * VFS ioctl capability
 *
 * \def VFS_OPS_GETDIRENT
 * VFS getdirent capability
 */
# define VFS_OPS_OPEN (1 << 0)
# define VFS_OPS_READ (1 << 1)
# define VFS_OPS_WRITE (1 << 2)
# define VFS_OPS_CLOSE (1 << 3)
# define VFS_OPS_UMOUNT (1 << 4)
# define VFS_OPS_MOUNT (1 << 5)
# define VFS_OPS_LOOKUP (1 << 6)
# define VFS_OPS_MKDIR (1 << 7)
# define VFS_OPS_MKNOD (1 << 8)
# define VFS_OPS_STAT (1 << 9)
# define VFS_OPS_IOCTL (1 << 10)
# define VFS_OPS_GETDIRENT (1 << 11)

/**
 * \brief   Initialize kernel VFS mechanism
 *
 * Mount temporary filesystem to / and create /dev directory.
 * Mount another temporary filesystem to /dev
 *
 * \return  0: Sucess
 * \return  -1: Failure
 */
int vfs_initialize(void);

#endif /* !VFS_H */

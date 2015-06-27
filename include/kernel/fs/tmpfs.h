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
 * \file    include/kernel/vfs/tmpfs.h
 * \brief   Temporary filesystem structures and variables
 *
 * \author  Baptiste Covolato
 */

#ifndef FS_VFS_TMPFS_H
# define FS_VFS_TMPFS_H

# include <kernel/zos.h>
# include <kernel/klist.h>
# include <kernel/types.h>

# include <kernel/fs/vfs.h>

# include <arch/spinlock.h>

# define TMPFS_DEV_ID -1

# define TMPFS_INODE_TABLE_INIT_SIZE 20
# define TMPFS_ROOT_INODE 0

# define TMPFS_BLOCK_SIZE (1 * KB)

# define TMPFS_TYPE_FILE 0x80000
# define TMPFS_TYPE_DIR 0x40000
# define TMPFS_TYPE_DEV 0x20000
# define TMPFS_TYPE_MOUNT 0x10000

/**
 * \brief   Represent an entry in the filesystem (file/directory/device/...)
 */
struct tmpfs_node {
    /**
     * \brief   The name of the entry
     */
    char *name;

    /**
     * \brief   The inode number of the entry
     */
    ino_t inode;

    /**
     * \brief   The UID of the entry
     */
    uint16_t uid;

    /**
     * \brief   The GID of the entry
     */
    uint16_t gid;

    /**
     * \brief   16 high bit are the type, 16 low bit are the permission
     */
    mode_t type_perm;

    /**
     * \brief   The size of the content, if the node is a device this field
     *          is the device id
     */
    size_t size;

    /**
     * \brief   List of directory entries
     */
    struct klist entries;

    /**
     * \brief   Lock used to access the file
     */
    spinlock_t lock;
};

/**
 * \brief   Represent an entry of a directory (available in the content of a
 *          directory).
 *
 * In directory content, this structure is followed by the name terminated
 * by '\0'
 */
struct tmpfs_dirent {
    /**
     * \brief   The inode of the entry
     */
    ino_t inode;

    /**
     * \brief   The name of the entry
     */
    char *name;

    /**
     * \brief   The next entry
     */
    struct klist next;
};

/**
 * \brief   Represent the tmpfs super block
 */
struct tmpfs_sb {
    /**
     * \brief   The inode table (the root inode is always inode 0)
     */
    struct tmpfs_node **inode_table;

    /**
     * \brief   The size of the inode table
     */
    size_t inode_table_size;

    /**
     * \brief   Lock used to ensure safety of the inode table
     */
    spinlock_t inode_table_lock;

    /**
     * \brief   Data used by the tmpfs to store the datas
     */
    void *blocks;

    /**
     * \brief   The number of blocks in the filesystem
     */
    size_t nb_blocks;

    /**
     * \brief   Keep track of free/allocated blocks
     * \todo    Make it a bit map
     */
    uint8_t *block_free;

    /**
     * \brief   Lock used to ensure \a block_free safety
     */
    spinlock_t lock_block;
};

extern struct fs_ops tmpfs_ops;

#endif /* !FS_VFS_TMPFS_H */

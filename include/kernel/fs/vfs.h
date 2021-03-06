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
 * \file    include/kernel/fs/vfs.h
 * \brief   Various virtual filesystem constants and prototype of
 *          initialization function
 *
 * \author  Baptiste Covolato
 */

#ifndef FS_VFS_H
# define FS_VFS_H

# include <kernel/klist.h>
# include <kernel/types.h>

# include <arch/spinlock.h>

/**
 * \brief   The maximum size of a filename
 */
# define VFS_MAX_FILENAME_SIZE 255
# define VFS_FS_MAX_NAMEL 15

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
 *
 * \def VFS_FS_CREATE
 * VFS fs create message identifier
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
# define VFS_FS_CREATE 13

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
 *
 * \def VFS_OPS_FS_CREATE
 * VFS fs create capability
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
# define VFS_OPS_FS_CREATE (1 << 12)

/**
 * \def VFS_PERM_OTHER_R
 * Represent read permission for other
 *
 * \def VFS_PERM_OTHER_W
 * Represent write permission for other
 *
 * \def VFS_PERM_OTHER_X
 * Represent execute permission for other
 */
# define VFS_PERM_OTHER_R 0x004
# define VFS_PERM_OTHER_W 0x002
# define VFS_PERM_OTHER_X 0x001

/**
 * \def VFS_PERM_GROUP_R
 * Represent read permission for group
 *
 * \def VFS_PERM_GROUP_W
 * Represent write permission for group
 *
 * \def VFS_PERM_GROUP_X
 * Represent execute permission for group
 */
# define VFS_PERM_GROUP_R 0x020
# define VFS_PERM_GROUP_W 0x010
# define VFS_PERM_GROUP_X 0x008

/**
 * \def VFS_PERM_USER_R
 * Represent read permission for user
 *
 * \def VFS_PERM_USER_W
 * Represent write permission for user
 *
 * \def VFS_PERM_USER_X
 * Represent execute permission for user
 */
# define VFS_PERM_USER_R 0x100
# define VFS_PERM_USER_W 0x080
# define VFS_PERM_USER_X 0x040

/**
 * \def VFS_PERM_OTHER_RX
 * Represent read and execute permission for other
 *
 * \def VFS_PERM_OTHER_RW
 * Represent read and write permission for other
 *
 * \def VFS_PERM_OTHER_RWX
 * Represent read, write and execute permission for other
 */
# define VFS_PERM_OTHER_RX (VFS_PERM_OTHER_R | VFS_PERM_OTHER_X)
# define VFS_PERM_OTHER_RW (VFS_PERM_OTHER_R | VFS_PERM_OTHER_W)
# define VFS_PERM_OTHER_RWX (VFS_PERM_OTHER_R | VFS_PERM_OTHER_W | \
                             VFS_PERM_OTHER_X)

/**
 * \def VFS_PERM_GROUP_RX
 * Represent read and execute permission for group
 *
 * \def VFS_PERM_GROUP_RW
 * Represent read and write permission for group
 *
 * \def VFS_PERM_GROUP_RWX
 * Represent read, write and execute permission for group
 */
# define VFS_PERM_GROUP_RX (VFS_PERM_GROUP_R | VFS_PERM_GROUP_X)
# define VFS_PERM_GROUP_RW (VFS_PERM_GROUP_R | VFS_PERM_GROUP_W)
# define VFS_PERM_GROUP_RWX (VFS_PERM_GROUP_R | VFS_PERM_GROUP_W | \
                             VFS_PERM_GROUP_X)

/**
 * \def VFS_PERM_USER_RX
 * Represent read and execute permission for user
 *
 * \def VFS_PERM_USER_RW
 * Represent read and write permission for user
 *
 * \def VFS_PERM_USER_RWX
 * Represent read, write and execute permission for user
 */
# define VFS_PERM_USER_RX (VFS_PERM_USER_R | VFS_PERM_USER_X)
# define VFS_PERM_USER_RW (VFS_PERM_USER_R | VFS_PERM_USER_W)
# define VFS_PERM_USER_RWX (VFS_PERM_USER_R | VFS_PERM_USER_W | \
                             VFS_PERM_USER_X)

/**
 * \def VFS_FTYPE_FILE
 * Numeric identifier of files
 *
 * \def VFS_FTYPE_DEV
 * Numeric identifier of devices
 *
 * \def VFS_FTYPE_DIR
 * Numeric identifier of directories
 */
# define VFS_FTYPE_FILE 0x8000
# define VFS_FTYPE_DEV 0x2000
# define VFS_FTYPE_DIR 0x4000

struct mount_entry;
struct resp_lookup;
struct stat;
struct req_rdwr;
struct process;
struct dirent;
struct file_operation;
struct fs_instance;
struct req_ioctl;

/**
 *  \brief  Attribute related to a file
 */
struct inode {
    /**
     *  \brief  The mode of the file
     */
    mode_t mode;

    /**
     *  \brief  The user id of the file
     */
    uid_t uid;

    /**
     *  \brief  The group id of the file
     */
    gid_t gid;

    /**
     *  \brief  The inode number of the file
     */
    ino_t inode;

    /**
     *  XXX: For now this is not true it contains the device of the file system
     *
     *  \brief  The device id of the file if the file is a device
     */
    dev_t dev;

    /**
     *  \brief  The size of the file
     */
    off_t size;

    /**
     *  \brief  The time of the last access to the file
     */
    time_t a_time;

    /**
     *  \brief  The last modification time of the file
     */
    time_t m_time;

    /**
     *  \brief  The last change time of the inode
     */
    time_t c_time;

    /**
     *  \brief  Use for reference counting
     */
    int ref;
};

struct file {
    /**
     *  \brief  Is the file used
     */
    int used;

    /**
     *  \brief  Attribute about the file
     */
    struct inode *inode;

    /**
     *  \brief  The current offset in the file
     */
    off_t offset;

    /**
     *  \brief  Pointers to operation callbacks
     */
    struct file_operation *f_ops;

    /**
     *  \brief  Private field used by drivers
     */
    void *private;

    /**
     *  \brief  The mount entry this file belongs to
     */
    struct mount_entry *mount;
};

/**
 *  \brief  File system operation.
 */
struct fs_operation {
    int (*lookup)(struct mount_entry *, const char *, uid_t, gid_t,
                  struct resp_lookup *);
    int (*mkdir)(struct mount_entry *, const char *, ino_t, uid_t, gid_t,
                 mode_t);
    int (*mknod)(struct mount_entry *, const char *, ino_t, uid_t, gid_t,
                 mode_t, dev_t);
    int (*stat)(struct mount_entry *, uid_t, gid_t, ino_t, struct stat *);
    int (*mount)(struct mount_entry *, ino_t, int);
    int (*getdirent)(struct mount_entry *, ino_t, struct dirent *, int);
};

/**
 *  \brief  File operation
 */
struct file_operation {
    int (*open)(struct file *, ino_t, pid_t, uid_t, gid_t, int, mode_t);
    int (*read)(struct file *, struct process *, struct req_rdwr *, void *buf);
    int (*write)(struct file *, struct process *, struct req_rdwr *,
                 void *buf);
    int (*ioctl)(struct file *, struct req_ioctl *, int *);
    int (*dup)(struct file *, struct file *);
    int (*close)(struct file *, ino_t);
};

/**
 *  \brief  Operations to manipulate creation/deletion of file systems
 */
struct fs_super_operation {
    int (*create)(struct fs_instance *fi, const char *device,
                  const char *mount_pt);
};

/**
 *  \brief  Represents a filesystem
 */
struct fs {
    /**
     *  \brief  Name of the filesystem
     */
    char name[VFS_FS_MAX_NAMEL];

    /**
     *  \brief  PID of the process that registered the file system
     */
    pid_t pid;

    /**
     *  \brief  Set of callbacks that will allow creation/deletion of a file
     *          system instance
     */
    struct fs_super_operation *fs_super_ops;

    /**
     *  \brief  Set of callbacks that will allow file system manipulations
     */
    struct fs_operation *fs_ops;

    /**
     *  \brief  Set of callbacks that will allow file manipulations
     */
    struct file_operation *f_ops;

    /**
     *  \brief  Private data for the file system
     */
    void *private;

    /**
     *  \brief  Lock to access this datas
     */
    spinlock_t lock;

    /**
     *  \brief  List of instances of the file system
     */
    struct klist instances;

    /**
     *  \brief  List of registered file systems
     */
    struct klist list;
};

/**
 *  \brief  Represents an actual mounted file system
 */
struct fs_instance {
    /**
     *  \brief  The file system type
     */
    struct fs *parent;

    /**
     *  \brief  Private data for the file system
     */
    void *private;

    /**
     *  \brief  List of instance of the same file system
     */
    struct klist list;
};

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

/**
 *  \brief  Initialize the file system mechanism
 *
 *  \return 0: Success
 */
int fs_initialize(void);

/**
 *  \brief  Get the file system from its name
 *
 *  \param  name    The name of the file system
 *
 *  \return The file system if found, NULL otherwise
 */
struct fs *fs_from_name(const char *name);

/**
 *  \brief  Register a new file system within the system
 *
 *  \param  name        The name of the file system
 *  \param  pid         The pid of the process that owns the file system
 *  \param  fs_sup_ops  Operations used to create/delete file system instances
 *  \param  fs_ops      Operations used to manipulate the file system
 *  \param  private     Private data
 *
 *  \return 0: Success
 *  \return -ENOMEM: Not enough memory
 *  \return -EEXIST: File system already exists with this name
 */
int fs_register(const char *name, pid_t pid,
                struct fs_super_operation *fs_sup_ops,
                struct fs_operation *fs_ops, struct file_operation *f_ops,
                void *private);

/**
 *  \brief  Unregister a file system
 *
 *  \param  name    The name of the file system
 *  \param  pid     The pid of the owner
 *
 *  \return 0: Success
 *  \return -ENOTENT: No such file system
 *  \return -EPERM: Operation not permitted
 */
int fs_unregister(const char *name, pid_t pid);

/**
 *  \brief  Create a new instance of a file system
 *
 *  \param  name        The name of the file system you want to instantiate
 *  \param  device      The device the file system will read/write from/to
 *  \param  mount_pt    The mount point
 *  \param  fi          This pointer will be set if everything went well
 *
 *  \return 0: Success
 *  \return -ENOENT: No such file system
 *  \return -ENOMEM: Not enough memory
 */
int fs_new_instance(const char *name, const char *device, const char *mount_pt,
                    struct fs_instance **fi);

/**
 *  \brief  Delete an instance of the file system
 *
 *  \param  instance    The instance you want to delete
 */
void fs_del_instance(struct fs_instance *instance);

/**
 *  \brief  Allocate a new inode
 *
 *  \param  mode    The mode of the inode (ie its permission)
 *
 *  \return The new inode if everything went well, NULL otherwise
 */
struct inode *inode_new(mode_t mode);

/**
 *  \brief  Add a reference on the inode
 *
 *  \param  inode   The inode you want to increment the reference
 */
static inline void inode_inc(struct inode *inode)
{
    ++inode->ref;
}

/**
 *  \brief  Delete an existing inode. Inodes use a reference counting
 *          mechanism, this will not always result in a deallocation
 *
 *  \param  inode   The inode you want to delete
 */
void inode_del(struct inode *inode);

#endif /* !FS_VFS_H */

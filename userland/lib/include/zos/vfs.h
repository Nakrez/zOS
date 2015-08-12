#ifndef ZOS_VFS_H
# define ZOS_VFS_H

# include <stdint.h>
# include <dirent.h>

# include <sys/types.h>
# include <sys/stat.h>

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
 *  \brief  See the structure that is in the kernel
 */
struct inode {
    mode_t mode;

    uid_t uid;
    gid_t gid;
    ino_t inode;
    dev_t dev;

    off_t size;

    time_t a_time;
    time_t m_time;
    time_t c_time;
};

/* Lookup request */
struct req_lookup {
    char *path;
    uint16_t path_size;
    uid_t uid;
    gid_t gid;
};

# define LOOKUP_RES_OK 0
# define LOOKUP_RES_ENTER_MOUNT 1
# define LOOKUP_RES_KO 2

/* Lookup response */
struct resp_lookup {
    int ret;
    int processed;
    struct inode inode;
};

/* Stat request */
struct req_stat {
    ino_t inode;
    uid_t uid;
    gid_t gid;
};

/* Stat response */
struct resp_stat {
    int ret;

    struct stat stat;
};

/* Open request */
struct req_open {
    ino_t inode;
    uid_t uid;
    gid_t gid;
    int flags;
    mode_t mode;
    pid_t pid;
};

/* Open response */
struct resp_open {
    int ret;
    ino_t inode;
};

/* Read/Write request */
struct req_rdwr {
    ino_t inode;

    size_t size;

    uint64_t off;

    void *data;
};

/* Read/Write response */
struct resp_rdwr {
    int ret;

    size_t size;
};

/* Close request */
struct req_close {
    ino_t inode;
};

/* Close response */
struct resp_close {
    int ret;
};

/* Ioctl request */
struct req_ioctl {
    ino_t inode;

    int request;

    int with_argp;

    int argp;
};

/* Ioctl response */
struct resp_ioctl {
    int ret;

    int modify_argp;

    int argp;
};

/* Dirent request */
struct req_getdirent {
    ino_t inode;

    int index;
};

/* Dirent response */
struct resp_getdirent {
    int ret;

    struct dirent dirent;
};

/* Mount request */
struct req_mount {
    ino_t inode;

    int mount_nb;
};

/* Mount response */
struct resp_mount {
    int ret;
};

int open_device(const char *device_name, int flags, mode_t mode);

int channel_create(const char *c_name);
int channel_open(const char *c_name);

int fs_register(const char *fs_name, int channel_fd, vop_t ops);
int fs_unregister(const char *fs_name);

#endif /* !ZOS_VFS_H */

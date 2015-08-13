#ifndef FS_VFS_MESSAGE_H
# define FS_VFS_MESSAGE_H

# include <kernel/types.h>
# include <kernel/klist.h>

# include <kernel/fs/vfs.h>
# include <kernel/fs/vfs/vops.h>
# include <kernel/fs/vfs/device.h>

# define MESSAGE_EXTRACT(type, msg) ((type *)(msg + 1))

# define RES_OK 0
# define RES_ENTER_MOUNT 1
# define RES_KO 2

struct message *message_alloc(size_t size);
void message_free(struct message *msg);

struct free_msg {
    struct message *msg;

    struct klist list;
};

struct message {
    uint32_t mid;

    size_t size;

    size_t max_size;
};

/* Lookup request */
struct req_lookup {
    char *path;
    uint16_t path_size;
    uid_t uid;
    gid_t gid;
};

/* Lookup response */
struct resp_lookup {
    int ret;
    int16_t processed;
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

#endif /* !FS_VFS_MESSAGE_H */

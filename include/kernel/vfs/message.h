#ifndef VFS_MESSAGE_H
# define VFS_MESSAGE_H

# include <kernel/types.h>
# include <kernel/klist.h>

struct free_msg {
    struct message *msg;

    struct klist list;
};

struct message {
    uint32_t mid;

    size_t size;

    size_t max_size;
};

struct msg_response {
    uint32_t req_id;

    int ret;
};

/* Open related message */
struct open_msg {
    char *path;
};

/* Read/Write related message */
struct rdwr_msg {
    uint32_t index;

    size_t size;

    size_t off;

    void *data;
};

/* Close related message */
struct close_msg {
    uint32_t index;
};

struct message *message_alloc(size_t size);
void message_free(struct message *msg);


/* New messages */

# define RES_OK 0
# define RES_ENTER_MOUNT 1
# define RES_KO 2

/* Lookup request */
struct req_lookup {
    char *path;
    uint16_t path_size;
    uint16_t uid;
    uint16_t gid;
};

struct resp_lookup {
    int ret;
    ino_t inode;
    int processed;
    int dev;
};

/* Open request */
struct req_open {
    ino_t inode;
    uint16_t uid;
    uint16_t gid;
    int flags;
    int mode;
};

/* Open response */
struct resp_open {
    int ret;
    ino_t inode;
    int dev;
};

#endif /* !VFS_MESSAGE_H */

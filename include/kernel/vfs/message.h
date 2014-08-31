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

struct answer_hdr {
    uint32_t req_id;
};

/* Open related message */
struct open_msg {
    char *path;
};

struct open_resp {
    struct answer_hdr hdr;

    int ret;
};

struct message *message_alloc(size_t size);
void message_free(struct message *msg);

#endif /* !VFS_MESSAGE_H */

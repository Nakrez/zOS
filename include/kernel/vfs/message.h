#ifndef VFS_MESSAGE_H
# define VFS_MESSAGE_H

# include <kernel/types.h>

# define VFS_OP_OPEN 1
# define VFS_OP_READ 2
# define VFS_OP_WRITE 3
# define VFS_OP_CLOSE 4

struct message {
    uint32_t mid;

    size_t size;
};

struct answer_hdr {
    uint32_t req_id;
};

#endif /* !VFS_MESSAGE_H */

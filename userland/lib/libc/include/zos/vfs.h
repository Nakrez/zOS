#ifndef ZOS_VFS_H
# define ZOS_VFS_H

# define VFS_OPS_OPEN (1 << 0)
# define VFS_OPS_READ (1 << 1)
# define VFS_OPS_WRITE (1 << 2)
# define VFS_OPS_CLOSE (1 << 3)

struct msg_response {
    uint32_t req_id;

    int ret;
};

struct open_msg {
    char *path;
};

#endif /* !ZOS_VFS_H */

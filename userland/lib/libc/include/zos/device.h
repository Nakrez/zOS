#ifndef ZOS_DEVICE_H
# define ZOS_DEVICE_H

# include <stdint.h>

# include <zos/vfs.h>

int device_create(const char *name, int uid, int gid, int perm, int ops);
int device_recv_request(int dev, char *buf, size_t size);

#endif /* !ZOS_DEVICE_H */

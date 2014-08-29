#ifndef ZOS_DEVICE_H
# define ZOS_DEVICE_H

# include <zos/vfs.h>

int device_create(const char *name, int uid, int gid, int perm, int ops);

#endif /* !ZOS_DEVICE_H */

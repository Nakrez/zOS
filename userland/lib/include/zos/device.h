#ifndef ZOS_DEVICE_H
# define ZOS_DEVICE_H

# include <stdint.h>

# include <zos/vfs.h>

dev_t device_create(int channel_fd, const char *name, int perm, vop_t ops);
int device_exists(const char *name);

#endif /* !ZOS_DEVICE_H */

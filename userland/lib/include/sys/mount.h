#ifndef I386_SYS_MOUNT_H
# define I386_SYS_MOUNT_H

# include <sys/types.h>

int mount(const char *fs_name, const char *device, const char *mount_pt);

#endif /* !I386_SYS_MOUNT_H */

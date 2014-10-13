#ifndef I386_SYS_MOUNT_H
# define I386_SYS_MOUNT_H

# include <sys/types.h>

int mount(dev_t dev, const char *target);

#endif /* !I386_SYS_MOUNT_H */

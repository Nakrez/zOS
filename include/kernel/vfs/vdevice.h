#ifndef VFS_VDEVICE_H
# define VFS_VDEVICE_H

# include <kernel/zos.h>

# include <kernel/vfs/vchannel.h>

# define VFS_MAX_DEVICE 255

struct vdevice {
    char *name;

    int active;

    int id;

    int pid;

    struct vchannel *channel;
};

int device_create(int pid, const char __user* name, struct vdevice **device);
struct vdevice *device_get(int dev);
int device_destroy(int pid, int dev);

#endif /* !VFS_VDEVICE_H */

#ifndef VFS_VDEVICE_H
# define VFS_VDEVICE_H

# include <kernel/zos.h>

# include <kernel/vfs/vchannel.h>
# include <kernel/vfs/vnode.h>

# define VFS_MAX_DEVICE 255

struct vdevice {
    char *name;

    int active;

    int id;

    int pid;

    int ops;

    struct vnode *node;

    struct vchannel *channel;
};

int device_create(int pid, const char __user* name, int ops,
                  struct vdevice **device);
struct vdevice *device_get(int dev);
int device_recv_request(int dev, char *buf, size_t size);

/* Send a response
 *
 * Return:
 *          0: Success
 *          -EINVAL: Device id is not ours our out of range
 *          -ENODEV: Device id does not belong to an active device
 *          -ENOMEM: Cannot allocate necessary memory
 */
int device_send_response(int dev, uint32_t req_id, char *buf, size_t size);

int device_destroy(int pid, int dev);

#endif /* !VFS_VDEVICE_H */

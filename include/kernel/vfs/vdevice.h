#ifndef VFS_VDEVICE_H
# define VFS_VDEVICE_H

# include <kernel/zos.h>

# include <kernel/vfs/vchannel.h>
# include <kernel/vfs/vnode.h>
# include <kernel/vfs/message.h>

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

/* Create a new device
 *
 * pid: The pid of the creating process
 * name: The name of the device
 * ops: Operation supported by the device
 * device: Resulting device if return value >= 0
 *
 * Return:
 *          device id: Success
 *          -EINVAL: Invalid ops
 *          -ENOMEM: Cannot allocate memory
 *          -EEXIST: Device already exists
 */
int device_create(int pid, const char __user* name, int ops,
                  struct vdevice **device);

/* Get device structure from device id
 *
 * dev: The device id
 *
 * Return:
 *          NULL: error
 *          struct vdevice *: corresponding to device id
 */
struct vdevice *device_get(int dev);

/* Receive a buffer from device inbox
 *
 * dev: Device id
 * buf: Reception buffer
 * size: Size to receive
 *
 * Return:
 *          Size of the buffer received: success
 *          -EINVAL: Wrong device id, or the device does not belong to your
 *                   process
 *          -ENODEV: Device does not exists
 */
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

int device_open(int dev, ino_t inode, uint16_t uid, uint16_t gid, int flags,
                mode_t mode);
int device_read(int dev, struct req_rdwr *, char *buf);

int device_destroy(int pid, int dev);

#endif /* !VFS_VDEVICE_H */

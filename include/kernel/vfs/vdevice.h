#ifndef VFS_VDEVICE_H
# define VFS_VDEVICE_H

# include <kernel/types.h>
# include <kernel/zos.h>

# include <kernel/vfs/vchannel.h>
# include <kernel/vfs/message.h>

# define VFS_MAX_DEVICE 255

struct vdevice {
    char *name;

    int active;

    dev_t id;

    int pid;

    vop_t ops;

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
dev_t device_create(pid_t pid, const char __user* name, vop_t ops,
                    struct vdevice **device);

/* Get device structure from device id
 *
 * dev: The device id
 *
 * Return:
 *          NULL: error
 *          struct vdevice *: corresponding to device id
 */
struct vdevice *device_get(dev_t dev);

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
int device_recv_request(dev_t dev, char *buf, size_t size);

/* Send a response
 *
 * Return:
 *          0: Success
 *          -EINVAL: Device id is not ours our out of range
 *          -ENODEV: Device id does not belong to an active device
 *          -ENOMEM: Cannot allocate necessary memory
 */
int device_send_response(dev_t dev, uint32_t req_id, char *buf, size_t size);

int device_open(dev_t dev, ino_t inode, pid_t pid, uid_t uid, gid_t gid,
                int flags, mode_t mode);

int device_read_write(struct process *process, dev_t dev, struct req_rdwr *req,
                      char *buf, int op);

int device_close(dev_t dev, ino_t inode);

int device_destroy(pid_t pid, dev_t dev);

#endif /* !VFS_VDEVICE_H */

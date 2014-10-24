#ifndef ZOS_DEVICE_H
# define ZOS_DEVICE_H

# include <stdint.h>

# include <zos/vfs.h>

dev_t device_create(const char *name, int perm, vop_t ops);
int device_recv_request(dev_t dev, char *buf, size_t size);
int device_send_response(dev_t dev, uint32_t req_id, void *buf, size_t size);

#endif /* !ZOS_DEVICE_H */

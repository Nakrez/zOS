#include <string.h>

#include <kernel/errno.h>
#include <kernel/kmalloc.h>

#include <kernel/vfs/vfs.h>
#include <kernel/vfs/vdevice.h>

#include <arch/spinlock.h>

static struct vdevice devices[VFS_MAX_DEVICE];
static spinlock_t device_lock = SPINLOCK_INIT;

int device_create(int pid, const char __user* name, int ops,
                  struct vdevice **device)
{
    int found = 0;

    if (!(ops & VFS_OPS_OPEN) || !(ops & VFS_OPS_CLOSE))
        return -EINVAL;

    *device = NULL;

    spinlock_lock(&device_lock);

    for (int i = 0; i < VFS_MAX_DEVICE; ++i)
    {
        /* FIXME: User access */
        if (devices[i].active && strcmp(devices[i].name, name) == 0)
            found = 1;
        if (!(*device) && !devices[i].active)
        {
            if (!(devices[i].name = kmalloc(strlen(name) + 1)))
            {
                spinlock_unlock(&device_lock);

                return -ENOMEM;
            }

            if (!(devices[i].channel = channel_create()))
            {
                kfree(devices[i].name);
                spinlock_unlock(&device_lock);

                return -ENOMEM;
            }

            /* FIXME: User access !!! */
            strcpy(devices[i].name, name);

            devices[i].id = i;
            devices[i].active = 1;
            devices[i].pid = pid;
            devices[i].ops = ops;

            *device = &devices[i];
        }
    }

    /* Device already exists */
    if (found && *device)
    {
        (*device)->active = 0;
        kfree((*device)->name);
        channel_destroy((*device)->channel);
        *device = NULL;

        spinlock_unlock(&device_lock);

        return -EEXIST;
    }

    spinlock_unlock(&device_lock);

    if (!(*device))
        return -ENOMEM;

    return (*device)->id;
}

struct vdevice *device_get(int dev)
{
    if (dev < 0 || dev >= VFS_MAX_DEVICE)
        return NULL;

    if (!devices[dev].active)
        return NULL;

    return &devices[dev];
}

int device_destroy(int pid, int dev)
{
    if (dev < 0 || dev >= VFS_MAX_DEVICE)
        return -EINVAL;

    if (!devices[dev].active)
        return -ENODEV;

    if (devices[dev].pid != pid)
        return -EINVAL;

    spinlock_lock(&device_lock);

    devices[dev].active = 0;

    kfree(devices[dev].name);

    spinlock_unlock(&device_lock);

    return 0;
}

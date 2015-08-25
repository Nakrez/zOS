#include <unistd.h>
#include <string.h>
#include <thread.h>

#include <zos/print.h>
#include <zos/device.h>

#include <driver/driver.h>

#include <arch/interrupt.h>

#include <kbd.h>
#include <buffer.h>

static int kbd_open(struct driver *driver, struct req_open *request,
                    ino_t *inode)
{
    (void) request;

    struct kbd *kbd = driver->private;

    *inode = 0;

    spinlock_init(&kbd->lock);

    if (kbd->opened)
        /* XXX: EBUSY */
        return -1;

    kbd->opened = 1;
    kbd->slave = -1;

    return 0;
}

static int kbd_read(struct driver *driver, struct req_rdwr *msg,
                    size_t *size_read)
{
    struct kbd *kbd = driver->private;
    struct input_event result;

    /* TODO: EINVAL */
    if (msg->size < sizeof (struct input_event))
        return -1;

    spinlock_lock(&kbd->lock);

    if (kbd->slave >= 0) {
        spinlock_unlock(&kbd->lock);
        /* TODO: EIO */
        return -1;
    }

    if (buffer_empty()) {
        memcpy(&kbd->req, msg, sizeof (struct req_rdwr));
        kbd->slave = msg->hdr.slave_id;

        spinlock_unlock(&kbd->lock);

        return DRV_NORESPONSE;
    }

    buffer_pop(&result);

    memcpy(msg->data, &result, sizeof (struct input_event));

    *size_read = sizeof (struct input_event);

    spinlock_unlock(&kbd->lock);

    return 0;
}

static int kbd_close(struct driver *driver, struct req_close *msg)
{
    (void) msg;

    struct kbd *kbd = driver->private;

    kbd->opened = 0;

    return 0;
}

static struct driver_ops kbd_ops = {
    .open = kbd_open,
    .read = kbd_read,
    .close = kbd_close,
};

int main(void)
{
    struct kbd kbd;
    int arch_tid;

    kbd.opened = 0;
    kbd.slave = -1;
    spinlock_init(&kbd.lock);

    if (driver_create("kbd", 0444, &kbd_ops, &kbd.driver) < 0)
    {
        uprint("Cannot register \"kbd\" device");
        return 2;
    }

    kbd.driver.private = &kbd;

    arch_tid = thread_create(interrupt_thread, 1, &kbd);

    if (arch_tid < 0)
    {
        uprint("Cannot create keyboard interrupt thread ...");
        return 1;
    }

    return driver_loop(&kbd.driver);
}

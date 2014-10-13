#include <unistd.h>
#include <string.h>
#include <thread.h>

#include <zos/print.h>
#include <zos/device.h>

#include <driver/driver.h>

#include <arch/interrupt.h>

#include <buffer.h>

/* Kbd driver can only be opened once */
static int kbd_opened = 0;

static int kbd_open(struct driver *driver, int mid, struct req_open *request,
                     ino_t *inode)
{
    (void) driver;
    (void) request;
    (void) mid;

    *inode = 0;

    if (!kbd_opened)
    {
        kbd_opened = 1;

        return 0;
    }

    /* TODO: EBUSY */

    return -1;
}

static int kbd_read(struct driver *driver, int mid, struct req_rdwr *msg,
                    size_t *size_read)
{
    (void) driver;
    (void) mid;

    struct input_event result;

    /* TODO: EINVAL */
    if (msg->size < sizeof (struct input_event))
        return -1;

    while (buffer_empty())
        ;

    buffer_pop(&result);

    memcpy(msg->data, &result, sizeof (struct input_event));

    *size_read = sizeof (struct input_event);

    return 0;
}

static int kbd_close(struct driver *driver, int mid, struct req_close *msg)
{
    (void) driver;
    (void) mid;
    (void) msg;

    kbd_opened = 0;

    return 0;
}

static struct driver_ops kbd_ops = {
    .open = kbd_open,
    .read = kbd_read,
    .close = kbd_close,
};

int main(void)
{
    struct driver kbd_driver;
    int arch_tid;

    arch_tid = thread_create(interrupt_thread, NULL);

    if (arch_tid < 0)
    {
        uprint("Cannot create keyboard interrupt thread ...");
        return 1;
    }

    if (driver_create("kbd", 0444, &kbd_ops, &kbd_driver) < 0)
    {
        uprint("Cannot register \"kbd\" device");
        return 2;
    }

    return driver_loop(&kbd_driver);
}

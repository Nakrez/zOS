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

static void kbd_open(struct driver *driver, int mid, struct open_msg *msg)
{
    (void) msg;

    if (!kbd_opened)
    {
        driver_send_response(driver, mid, 0);
        kbd_opened = 1;
    }
    else
        /* TODO: EBUSY */
        driver_send_response(driver, mid, -1);
}

static void kbd_read(struct driver *driver, int mid, struct rdwr_msg *msg)
{
    struct input_event result;

    if (msg->size < sizeof (struct input_event))
    {
        /* TODO: EINVAL */
        driver_send_response(driver, mid, -1);

        return;
    }

    while (buffer_empty())
        ;

    buffer_pop(&result);

    memcpy(msg->data, &result, sizeof (struct input_event));

    driver_send_response(driver, mid, sizeof (struct input_event));
}

static void kbd_close(struct driver *driver, int mid, struct close_msg *msg)
{
    (void) msg;

    kbd_opened = 0;

    driver_send_response(driver, mid, 0);
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

    if (driver_create("kbd", 0, 0, 0444, &kbd_ops, &kbd_driver) < 0)
    {
        uprint("Cannot register \"kbd\" device");
        return 2;
    }

    return driver_loop(&kbd_driver);
}

#include <unistd.h>
#include <string.h>
#include <thread.h>

#include <zos/print.h>
#include <zos/device.h>

#include <driver/driver.h>

#include <arch/interrupt.h>

static void kbd_read(struct driver *driver, int mid, struct rdwr_msg *msg)
{
    char *data = msg->data;

    data[0] = 'T';
    data[1] = 'E';
    data[2] = 'S';
    data[3] = 'T';
    data[4] = 0;

    driver_send_response(driver, mid, msg->size);
}

static void kbd_write(struct driver *driver, int mid, struct rdwr_msg *msg)
{
    driver_send_response(driver, mid, msg->size);
}

static struct driver_ops kbd_ops = {
    NULL,
    kbd_read,
    kbd_write,
    NULL,
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

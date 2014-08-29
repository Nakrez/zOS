#include <unistd.h>
#include <string.h>
#include <thread.h>

#include <zos/print.h>
#include <zos/device.h>

#include <driver/driver.h>

#include <arch/interrupt.h>

static struct driver_ops kbd_ops = {
    NULL,
    NULL,
    NULL,
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

    while (1)
        sleep(1);

    return 0;
}

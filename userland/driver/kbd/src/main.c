#include <unistd.h>
#include <string.h>
#include <thread.h>

#include <zos/print.h>
#include <zos/device.h>

#include <arch/interrupt.h>

int main(void)
{
    int arch_tid;
    int dev_id;

    arch_tid = thread_create(interrupt_thread, NULL);

    if (arch_tid < 0)
    {
        uprint("Cannot create keyboard interrupt thread ...");
        return 1;
    }

    dev_id = device_create("kbd", 0, 0, 0444);

    if (dev_id < 0)
    {
        uprint("Cannot register \"kbd\" device");
        return 2;
    }

    while (1)
        sleep(1);

    return 0;
}

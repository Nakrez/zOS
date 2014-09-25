#include <unistd.h>
#include <thread.h>

#include <zos/print.h>

#include "ide.h"
#include "driver.h"

static struct ide_controller ide;

int main(void)
{
    int tid;

    if (ide_detect(&ide) < 0)
    {
        uprint("ATA: No IDE controller found. Bye!");

        return 1;
    }

    if (!ide_initialize(&ide))
    {
        uprint("ATA: No disk have been detected. Bye!");

        return 2;
    }

    uprint("ATA: configuration detected, creating devices ...");

    for (int i = 0; i < 4; ++i)
    {
        if (ide.devices[i].exists)
        {
            tid = thread_create(driver_device_thread, &ide.devices[i]);

            if (tid < 0)
                uprint("ATA: Fail to start thread");
        }
    }

    while (1)
        sleep(1);

    return 0;
}

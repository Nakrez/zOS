#include <unistd.h>

#include <zos/print.h>

#include "ide.h"

static struct ide_controller ide;

int main(void)
{
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

    while (1)
        sleep(1);

    return 0;
}

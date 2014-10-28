#include <unistd.h>
#include <thread.h>

#include <zos/print.h>

#include "vbe.h"

int main(void)
{
    uprint("Video: Launching driver");

    if (vbe_initialize() < 0)
    {
        uprint("Video: Failed to initialize VBE extensions");

        return 1;
    }

    uprint("TEST");

    return 0;
}

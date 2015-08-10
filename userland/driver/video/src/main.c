#include <unistd.h>
#include <stdlib.h>

#include <zos/print.h>
#include <zos/video.h>

#include "driver.h"

/* Must be implemented by each architecture */
int video_initialize(struct video *);

int main(void)
{
    struct video *driver_implem;

    uprint("Video: Launching driver");

    if (!(driver_implem = malloc(sizeof (struct video))))
    {
        uprint("Video: Out of memory");

        return 1;
    }

    if (video_initialize(driver_implem) < 0)
    {
        free(driver_implem);

        return 1;
    }

    driver_implem->opened = 0;

    uprint("Video: Initialization successful");

    return video_driver_run(driver_implem);
}

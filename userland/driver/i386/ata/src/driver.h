#ifndef DRIVER_H
# define DRIVER_H

# include "ide.h"

struct ata_private {
    struct ide_device *device;

    /* -1 signify the all disk */
    int partition;
};

void driver_device_thread(void *param);

#endif /* !DRIVER_H */

#ifndef DRIVER_H
# define DRIVER_H

# include "ide.h"

struct ata_private {
    struct ide_device *device;

    /* -1 signify the all disk */
    int partition;
};

void driver_device_thread(int argc, void *argv[]);

#endif /* !DRIVER_H */

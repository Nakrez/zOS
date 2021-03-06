#include <stdio.h>
#include <string.h>
#include <thread.h>

#include <zos/print.h>

#include <driver/driver.h>

#include "driver.h"
#include "ata.h"

# define MIN(a, b) ((a < b) ? a : b)

# define ALIGN_SUP(a, b) ((a + b - 1) & (~(b - 1)))
# define ALIGN_INF(a, b) (a % b ? ALIGN_SUP(a, b) - b : a)

static int read_with_offset(struct driver *driver, struct req_rdwr *msg,
                            uint64_t *lba, size_t *read)
{
    char buf[ATA_SECTOR_SIZE];
    size_t read_size = 0;
    struct ata_private *private_data = driver->private;
    struct ide_device *device = private_data->device;

    if (!ata_rdwr(device, ATA_OP_READ, *lba, buf, ATA_SECTOR_SIZE, 1))
        return -1;

    read_size = (ATA_SECTOR_SIZE - msg->off % ATA_SECTOR_SIZE);

    memcpy(msg->data, buf + ATA_SECTOR_SIZE - read_size,
           MIN(read_size, msg->size));

    *read = msg->size;

    if (read_size >= msg->size)
        return -1;

    *read = read_size;

    msg->data += read_size;
    msg->size -= read_size;

    ++(*lba);

    return 0;
}

static int read_blocks(struct driver *driver, struct req_rdwr *msg,
                       uint64_t *lba, size_t *read)
{
    size_t read_size;
    struct ata_private *private_data = driver->private;
    struct ide_device *device = private_data->device;

    if (!ata_rdwr(device, ATA_OP_READ, *lba, msg->data, ATA_SECTOR_SIZE,
                  msg->size / ATA_SECTOR_SIZE))
        return -1;

    read_size = ALIGN_INF(msg->size, ATA_SECTOR_SIZE);
    *lba += read_size / ATA_SECTOR_SIZE;
    msg->size -= read_size;
    msg->data += read_size;

    *read += read_size;

    return 0;
}

static int read_end(struct driver *driver, struct req_rdwr *msg, uint64_t lba,
                    size_t *read)
{
    char buf[ATA_SECTOR_SIZE];
    struct ata_private *private_data = driver->private;
    struct ide_device *device = private_data->device;

    if (!ata_rdwr(device, ATA_OP_READ, lba, buf, ATA_SECTOR_SIZE, 1))
        return -1;

    memcpy(msg->data, buf, msg->size);
    *read += msg->size;

    return 0;
}

static int ata_global_read(struct driver *driver, struct req_rdwr *msg,
                           size_t *size_read)
{
    size_t read_size = 0;
    int ret;
    struct ata_private *private_data = driver->private;
    uint64_t lba = 0;

    if (private_data->partition >= 0)
        lba = private_data->device->partitions[private_data->partition].start;

    if (private_data->device->infos.configuration.is_atapi)
        return -1;

    lba += msg->off / ATA_SECTOR_SIZE;

    /*
     * If the offset is inside a block,
     * read it and fill the beginning of the buffer with it
     */
    if (msg->off % ATA_SECTOR_SIZE) {
        ret = read_with_offset(driver, msg, &lba, &read_size);
        if (ret < 0)
            goto error;
    }

    /* Read blocks directly in the buffer */
    if (msg->size / ATA_SECTOR_SIZE > 0) {
        ret = read_blocks(driver, msg, &lba, &read_size);
        if (ret < 0)
            goto error;
    }

    /* Data left to read but < ATA_SECTOR_SIZE */
    if (msg->size % ATA_SECTOR_SIZE) {
        ret = read_end(driver, msg, lba, &read_size);
        if (ret < 0)
            goto error;
    }

    *size_read = read_size;

    return 0;

error:
    if (read_size) {
        *size_read = read_size;
        return 0;
    }

    return -1;
}

static struct driver_ops ata_ops = {
    .read = ata_global_read,
};

static void driver_partition_thread(int argc, void *argv[])
{
    char name[20];
    struct driver device_driver;
    struct ata_private device_private;

    if (argc != 2)
    {
        uprint("Partition thread was not called with proper parameters");

        return;
    }

    device_private.device = argv[0];
    device_private.partition = (int)argv[1];
    device_driver.private = &device_private;

    sprintf(name, "ata-disk%ip%i", device_private.device->id,
            device_private.partition + 1);

    if (driver_create(name, 0600, &ata_ops, &device_driver) < 0)
    {
        uprint("Cannot spawn ata partition device driver");

        return;
    }

    driver_loop(&device_driver);
}

void driver_device_thread(int argc, void *argv[])
{
    (void) argc;

    char ata_name[10];
    struct driver device_driver;
    struct ata_private device_private;
    struct ide_device *device = argv[0];

    device_private.device = device;
    device_private.partition = -1;

    for (int i = 0; i < 4; ++i)
    {
        if (device->partitions[i].present)
            thread_create(driver_partition_thread, 2, device, i);
    }

    sprintf(ata_name, "ata-disk%i", device->id);

    device_driver.private = &device_private;

    if (driver_create(ata_name, 0600, &ata_ops, &device_driver) < 0)
    {
        uprint("Cannot spawn ata device driver");

        return;
    }

    driver_loop(&device_driver);
}

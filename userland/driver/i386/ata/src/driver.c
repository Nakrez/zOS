#include <stdio.h>
#include <string.h>

#include <zos/print.h>

#include <driver/driver.h>

#include "driver.h"
#include "ata.h"

# define MIN(a, b) ((a < b) ? a : b)

# define ALIGN_SUP(a, b) ((a + b - 1) & (~(b - 1)))
# define ALIGN_INF(a, b) (a % b ? ALIGN_SUP(a, b) - b : a)

static size_t read_with_offset(struct driver *driver, int mid,
                               struct rdwr_msg *msg, uint64_t *lba)
{
    char buf[ATA_SECTOR_SIZE];
    size_t read_size = 0;
    struct ata_private *private_data = driver->private;
    struct ide_device *device = private_data->device;

    if (!ata_rdwr(device, ATA_OP_READ, *lba, buf, ATA_SECTOR_SIZE, 1))
    {
        driver_send_response(driver, mid, -1);

        return 0;
    }

    read_size = (ATA_SECTOR_SIZE - msg->off % ATA_SECTOR_SIZE);

    memcpy(msg->data, buf + ATA_SECTOR_SIZE - read_size,
           MIN(read_size, msg->size));

    if (read_size >= msg->size)
    {
        driver_send_response(driver, mid, msg->size);

        return 0;
    }

    msg->data += read_size;
    msg->size -= read_size;

    lba += ATA_SECTOR_SIZE;

    return read_size;
}

static size_t read_blocks(struct driver *driver, struct rdwr_msg *msg,
                          uint64_t *lba)
{
    size_t read_size;
    struct ata_private *private_data = driver->private;
    struct ide_device *device = private_data->device;

    if (!ata_rdwr(device, ATA_OP_READ, *lba, msg->data, ATA_SECTOR_SIZE,
                  msg->size / ATA_SECTOR_SIZE))
        return 0;

    read_size = ALIGN_INF(msg->size, ATA_SECTOR_SIZE);
    *lba += read_size;
    msg->size -= read_size;
    msg->data += read_size;

    return read_size;
}

static size_t read_end(struct driver *driver, struct rdwr_msg *msg,
                       uint64_t lba)
{
    char buf[ATA_SECTOR_SIZE];
    size_t read_size;
    struct ata_private *private_data = driver->private;
    struct ide_device *device = private_data->device;

    if (!ata_rdwr(device, ATA_OP_READ, lba, buf, ATA_SECTOR_SIZE, 1))
        return 0;

    memcpy(msg->data, buf, msg->size);
    read_size = msg->size;

    return read_size;
}

static void ata_global_read(struct driver *driver, int mid,
                            struct rdwr_msg *msg)
{
    size_t read_size = 0;
    size_t ret;
    struct ata_private *private_data = driver->private;
    uint64_t lba;

    if (private_data->partition < 0)
        lba = 0;
    else
        lba = private_data->device->partitions[private_data->partition].start;

    if (private_data->device->infos.configuration.is_atapi)
        driver_send_response(driver, mid, -1);
    else
    {
        lba += ALIGN_INF(msg->off, ATA_SECTOR_SIZE);

        /*
         * If the offset is inside a block,
         * read it and fill the beginning of the buffer with it
         */
        if (msg->off % ATA_SECTOR_SIZE)
        {
            if (!(ret = read_with_offset(driver, mid, msg, &lba)))
                return;

            read_size += ret;
        }

        /* Read blocks directly in the buffer */
        if (msg->size / ATA_SECTOR_SIZE > 0)
        {
            if (!(ret = read_blocks(driver, msg, &lba)))
                goto error;

            read_size += ret;
        }

        /* Data left to read but < ATA_SECTOR_SIZE */
        if (msg->size % ATA_SECTOR_SIZE)
        {
            if (!(ret = read_end(driver, msg, lba)))
                goto error;

            read_size += ret;
        }

        driver_send_response(driver, mid, read_size);
    }

    return;

error:
    if (read_size)
        driver_send_response(driver, mid, read_size);
    else
        driver_send_response(driver, mid, -1);
}

static struct driver_ops ata_ops = {
    .read = ata_global_read,
};

static void driver_partition_thread(void *param)
{
    (void)param;

    while (1)
        ;
}

void driver_device_thread(void *param)
{
    char ata_name[10];
    struct driver device_driver;
    struct ata_private device_private;
    struct ide_device *device = param;

    device_private.device = device;
    device_private.partition = -1;

    for (int i = 0; i < 4; ++i)
    {
        if (device->partitions[i].present)
        {

        }
    }

    sprintf(ata_name, "ata-disk%i", device->id);

    device_driver.private = &device_private;

    if (driver_create(ata_name, 0, 0, 0600, &ata_ops, &device_driver) < 0)
    {
        uprint("Cannot spawn ata device driver");

        return;
    }

    driver_loop(&device_driver);
}

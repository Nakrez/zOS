#include "ata.h"

#include <sys/io.h>

#include <zos/print.h>

static const uint8_t translation_cmd[4][2] =
{
    { ATA_CMD_READ_PIO, ATA_CMD_READ_PIO_EXT },
    { ATA_CMD_WRITE_PIO, ATA_CMD_WRITE_PIO_EXT },
    { ATA_CMD_READ_DMA, ATA_CMD_READ_DMA_EXT },
    { ATA_CMD_WRITE_DMA, ATA_CMD_WRITE_DMA_EXT },
};


static uint8_t ata_op_to_cmd(struct ide_device *device, int op)
{
    int dma = 0;

    return translation_cmd[(op + dma * 2) - 1][device->infos.features.big_lba];
}

static int ata_rdwr_pio(struct ide_device *device, int op, void *buf,
                        size_t sec_size, size_t sec_count)
{
    uint16_t *buffer = buf;

    for (size_t i = 0; i < sec_count; ++i)
    {
        if (!ide_wait_status(device, ATA_TIMEOUT, ATA_SR_RDY, ATA_SR_BSY))
            return 0;

        if (op == ATA_OP_READ)
            ide_read_words(device, ATA_REG_DATA, buffer,
                           sec_size / sizeof (uint16_t));
        else
            ide_write_words(device, ATA_REG_DATA, buffer,
                            sec_size / sizeof (uint16_t));

        buffer += sec_size / sizeof (uint16_t);
    }

    return 1;
}

static int ata_prepare_command(struct ide_device *device, uint8_t cmd,
                               uint64_t lba, size_t sec_count)
{
    if (device->infos.features.big_lba)
        ide_write_reg(device, ATA_REG_HDDSEL,
                      0xE0 | ((device->id & 0x1) << 4));
    else if (device->infos.capabilities.lba)
        ide_write_reg(device, ATA_REG_HDDSEL,
                      0xE0 | ((device->id & 0x1) << 4) | ((lba >> 24) & 0x0F));

    ide_wait(device);

    ide_write_reg(device, ATA_REG_CONTROL, IDE_NIEN);

    if (device->infos.features.big_lba)
    {
        outb(device->chan->io_base + ATA_REG_SECCOUNT0, (sec_count >> 8));

        outb(device->chan->io_base + ATA_REG_LBA0, (lba & 0xFF000000) >> 24);
        outb(device->chan->io_base + ATA_REG_LBA1, (lba & 0xFF00000000) >> 32);
        outb(device->chan->io_base + ATA_REG_LBA2,
             (lba & 0xFF0000000000) >> 40);
        outb(device->chan->io_base + ATA_REG_SECCOUNT0, sec_count & 0xFF);

        outb(device->chan->io_base + ATA_REG_LBA0, lba & 0xFF);
        outb(device->chan->io_base + ATA_REG_LBA1, (lba & 0xFF00) >> 8);
        outb(device->chan->io_base + ATA_REG_LBA2, (lba & 0xFF0000) >> 16);
    }
    else if (device->infos.capabilities.lba)
    {
        ide_write_reg(device, ATA_REG_SECCOUNT0, sec_count & 0xFF);

        ide_write_reg(device, ATA_REG_LBA0, lba & 0xFF);
        ide_write_reg(device, ATA_REG_LBA1, (lba & 0xFF00) >> 8);
        ide_write_reg(device, ATA_REG_LBA2, (lba & 0xFF0000) >> 16);
    }

    ide_write_reg(device, ATA_REG_CMD, cmd);

    return 1;
}

int ata_rdwr(struct ide_device *device, int op, uint64_t lba, void *buf,
             size_t sec_size, size_t sec_count)
{
    uint8_t cmd = ata_op_to_cmd(device, op);

    if (!ata_prepare_command(device, cmd, lba, sec_count))
        return 0;

    switch (cmd)
    {
        case ATA_CMD_READ_PIO:
        case ATA_CMD_WRITE_PIO:
        case ATA_CMD_READ_PIO_EXT:
        case ATA_CMD_WRITE_PIO_EXT:
            return ata_rdwr_pio(device, op, buf, sec_size, sec_count);
        /* DMA not supported yet */
        default:
            return 0;
    }

    return 1;
}

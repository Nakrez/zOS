#include <unistd.h>

#include <zos/print.h>

#include <sys/io.h>

#include "ide.h"

static uint16_t pci_cfg_readw(int bus, int dev, int fun, uint8_t off)
{
    uint32_t address = 0x80000000;
    uint16_t res;

    address |= (bus << 16) | (dev << 11) | (fun << 8) | (off & 0xFC);

    outl(PCI_CMD, address);

    res = (inl(PCI_DATA) >> ((off & 2) * 8) & 0xFFFF);

    return res;
}

int ide_detect(struct ide_controller* ctrl)
{
    uint16_t class;

    /* Scan the PCI bus to identify the IDE controller */
    for (int bus = 0; bus < 256; ++bus)
    {
        for (int device = 0; device < 32; ++device)
        {
            for (int fun = 0; fun < 8; ++fun)
            {
                ctrl->vendor_id = pci_cfg_readw(bus, device, fun, 0);

                if (ctrl->vendor_id != 0xFFFF)
                {
                    class = pci_cfg_readw(bus, device, fun, 0xA);

                    if (((class >> 8) & 0xFF) == MASS_STORAGE_CLASS &&
                        (class & 0xFF) == IDE_SUBCLASS)
                    {
                        ctrl->device_id = pci_cfg_readw(bus, device, fun, 0x2);

                        ctrl->channels[ATA_PRIMARY].io_base = 0x1F0;
                        ctrl->channels[ATA_PRIMARY].io_ctrl = 0x3F4;
                        ctrl->channels[ATA_SECONDARY].io_base = 0x170;
                        ctrl->channels[ATA_SECONDARY].io_ctrl = 0x374;

                        return 0;
                    }
                }
            }
        }
    }

    return -1;
}

uint8_t ide_read_reg(struct ide_device *device, uint16_t reg)
{
    uint8_t res;

    /* LBA 48 */
    if (reg > ATA_REG_CMD && reg < ATA_REG_CONTROL)
        ide_write_reg(device, ATA_REG_CONTROL, 0x80 | IDE_NIEN);

    if (reg < ATA_REG_SECCOUNT1)
        res = inb(device->chan->io_base + reg);
    else if (reg < ATA_REG_CONTROL)
        res = inb(device->chan->io_base + reg - 0x06);
    else
        res = inb(device->chan->io_ctrl + reg - 0x0A);

    /* LBA 48 */
    if (reg > ATA_REG_CMD && reg < ATA_REG_CONTROL)
        ide_write_reg(device, ATA_REG_CONTROL, IDE_NIEN);

    return res;
}

void ide_write_reg(struct ide_device *device, uint16_t reg, uint8_t data)
{
    /* LBA 48 */
    if (reg > ATA_REG_CMD && reg < ATA_REG_CONTROL)
        ide_write_reg(device, ATA_REG_CONTROL, 0x80 | IDE_NIEN);

    if (reg < ATA_REG_SECCOUNT1)
        outb(device->chan->io_base + reg, data);
    else if (reg < ATA_REG_CONTROL)
        outb(device->chan->io_base + reg - 0x06, data);
    else
        outb(device->chan->io_ctrl + reg - 0x0A, data);

    /* LBA 48 */
    if (reg > ATA_REG_CMD && reg < ATA_REG_CONTROL)
        ide_write_reg(device, ATA_REG_CONTROL, IDE_NIEN);
}

void ide_wait(struct ide_device *device)
{
    ide_read_reg(device, ATA_REG_STATUS);
    ide_read_reg(device, ATA_REG_STATUS);
    ide_read_reg(device, ATA_REG_STATUS);
    ide_read_reg(device, ATA_REG_STATUS);
}

void ide_read_words(struct ide_device *device, int reg, uint16_t *buf,
                    size_t size)
{
    for (size_t i = 0; i < size; ++i)
        buf[i] = inw(device->chan->io_base + reg);
}

void ide_write_words(struct ide_device *device, int reg, uint16_t *buf,
                     size_t size)
{
    for (size_t i = 0; i < size; ++i)
        outw(device->chan->io_base + reg, buf[i]);
}

int ide_wait_status(struct ide_device *device, int timeout, uint8_t check_true,
                    uint8_t check_false)
{
    int elapsed = 0;
    uint8_t status;

    while (elapsed < timeout)
    {
        status = ide_read_reg(device, ATA_REG_STATUS);

        if (!(status & check_false) && (status & check_true) == check_true)
            return 1;

        usleep(10);
        elapsed += 10;
    }

    return 0;
}

static int ide_device_identify(struct ide_device *device, uint32_t command)
{
    uint8_t status;

    /* Select driver */
    ide_write_reg(device, ATA_REG_HDDSEL, (device->id & 0x1) << 4);

    ide_wait(device);

    /* Disable interruption */
    ide_write_reg(device, ATA_REG_CONTROL, IDE_NIEN);

    /* Send the identification command */
    ide_write_reg(device, ATA_REG_CMD, command);

    status = ide_read_reg(device, ATA_REG_STATUS);

    if (status == 0)
        return 0;

    ide_wait(device);

    if (!ide_wait_status(device, ATA_TIMEOUT, ATA_SR_DRQ, ATA_SR_BSY))
        return 0;

    /* Read configuration */
    ide_read_words(device, ATA_REG_DATA, (uint16_t *)&device->infos, 512);

    /* Wait for DRQ and BSY flags to clear */
    if (!ide_wait_status(device, ATA_TIMEOUT, 0, ATA_SR_DRQ | ATA_SR_BSY))
        return 0;

    return 1;
}

static void ide_device_initialize(struct ide_device *device)
{
    if (!ide_device_identify(device, ATA_CMD_IDENTIFY))
    {
        if (!ide_device_identify(device, ATA_CMD_IDENTIFY_PACKET))
            return;
    }

    if (device->infos.configuration.is_atapi)
    {
        uprint("ATA: ATAPI detected but not supported yet");

        return;
    }

    device->exists = 1;

    return;
}

int ide_initialize(struct ide_controller *ctrl)
{
    int ret = 0;

    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            ctrl->devices[i * 2 + j].exists = 0;
            ctrl->devices[i * 2 + j].chan = &ctrl->channels[i];
            ctrl->devices[i * 2 + j].id = i * 2 + j;

            ide_device_initialize(&ctrl->devices[i * 2 + j]);

            if (ctrl->devices[i * 2 + j].exists)
                ++ret;
        }
    }

    return ret;
}

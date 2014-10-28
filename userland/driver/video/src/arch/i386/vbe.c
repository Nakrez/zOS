#include <unistd.h>
#include <sys/io.h>
#include <sys/mman.h>

#include <zos/video.h>
#include <zos/print.h>

#include "vbe.h"

static uint16_t bga_read_register(uint16_t index)
{
    outw(VBE_DISPI_IOPORT_INDEX, index);

    return inw(VBE_DISPI_IOPORT_DATA);
}

static void bga_write_register(uint16_t index, uint16_t data)
{
    outw(VBE_DISPI_IOPORT_INDEX, index);
    outw(VBE_DISPI_IOPORT_DATA, data);
}

static void bga_bank_set(uint16_t bank)
{
    bga_write_register(VBE_DISPI_INDEX_BANK, bank);
}

int video_initialize(struct video *video)
{
    uint16_t vbe_index = bga_read_register(VBE_DISPI_INDEX_ID);

    if (vbe_index < VBE_DIPSI_ID0 || vbe_index > VBE_DIPSI_ID5)
    {
        uprint("Video: VBE Extended version not supported");

        return -1;
    }

    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    /* bga_write_register(VBE_DISPI_INDEX_XRES, 1024); */
    /* bga_write_register(VBE_DISPI_INDEX_YRES, 768); */
    /* bga_write_register(VBE_DISPI_INDEX_BPP, 32); */
    /* bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED); */

    video->xres = 0;
    video->yres = 0;
    video->bpp = 0;

    video->private = NULL;

    video->read = NULL;
    video->write = NULL;
    video->xres_set = NULL;
    video->yres_set = NULL;
    video->bpp_set = NULL;
    video->enable = NULL;
    video->disable = NULL;

    return 0;
}

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
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

static int vbe_xres_set(struct video *video, int xres)
{
    (void)video;

    bga_write_register(VBE_DISPI_INDEX_XRES, xres);

    return 0;
}

static int vbe_yres_set(struct video *video, int yres)
{
    (void) video;

    bga_write_register(VBE_DISPI_INDEX_YRES, yres);

    return 0;
}

static int vbe_bpp_set(struct video *video, int bpp)
{
    (void) video;

    bga_write_register(VBE_DISPI_INDEX_BPP, bpp);

    return 0;
}

static int vbe_enable(struct video *video)
{
    struct vbe *vbe = video->private;

    if (vbe->buffer)
        free(vbe->buffer);

    if (!(vbe->buffer = malloc(video->xres * video->yres * video->bpp / 8)))
        return 1;

    memset(vbe->buffer, 0, video->xres * video->yres * video->bpp / 8);

    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED);

    return 0;
}

static int vbe_disable(struct video *video)
{
    struct vbe *vbe = video->private;

    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);

    free(vbe->buffer);
    vbe->buffer = NULL;

    return 0;
}

int video_initialize(struct video *video)
{
    struct vbe *vbe;
    uint16_t vbe_index = bga_read_register(VBE_DISPI_INDEX_ID);

    if (vbe_index < VBE_DIPSI_ID0 || vbe_index > VBE_DIPSI_ID5)
    {
        uprint("Video: VBE Extended version not supported");

        return -1;
    }

    if (!(vbe = malloc(sizeof (struct vbe))))
        return -1;

    if (!(vbe->phy_mem = mmap_physical((void *)0xA0000, 64 * 1024)))
    {
        free(vbe);

        return -1;
    }

    video->xres = 0;
    video->yres = 0;
    video->bpp = 0;

    video->private = vbe;

    video->read = NULL;
    video->write = NULL;
    video->xres_set = vbe_xres_set;
    video->yres_set = vbe_yres_set;
    video->bpp_set = vbe_bpp_set;
    video->enable = vbe_enable;
    video->disable = vbe_disable;

    return 0;
}

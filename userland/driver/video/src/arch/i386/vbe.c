#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/io.h>
#include <sys/mman.h>

#include <zos/video.h>
#include <zos/print.h>

#include "vbe.h"
#include "../../font.h"

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

    vbe->bank = 0;
    bga_bank_set(vbe->bank);

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

static void vbe_flush_buffer(struct vbe *vbe, uint32_t pos, size_t size)
{
    uint32_t bank = pos / VBE_BANK_SIZE;
    uint32_t bank_pos = pos % VBE_BANK_SIZE;

    if (vbe->bank != bank)
    {
        vbe->bank = bank;
        bga_bank_set(bank++);
    }

    while (size)
    {
        vbe->phy_mem[bank_pos++] = vbe->buffer[pos++];

        if (bank_pos == VBE_BANK_SIZE)
        {
            vbe->bank = bank;
            bga_bank_set(bank++);
            bank_pos = 0;
        }

        --size;
    }
}

static void vbe_scroll(struct video *video)
{
    struct vbe *vbe = video->private;
    size_t buffer_size = video->xres * video->yres * video->bpp / 8;
    size_t line_size = (video->xres) * video->bpp / 8 * FONT_HEIGHT;

    memcpy(vbe->buffer, vbe->buffer + line_size, buffer_size - line_size);

    memset(vbe->buffer + buffer_size - line_size, 0, line_size);

    vbe_flush_buffer(vbe, 0, buffer_size);

    --vbe->cursor.y;
}

static void vbe_put_char(struct video *video, char c)
{
    struct vbe *vbe = video->private;
    uint32_t buffer_pos;

    buffer_pos = (vbe->cursor.y * video->xres * FONT_HEIGHT +
                  vbe->cursor.x * FONT_WIDTH) * video->bpp / 8;

    switch (c)
    {
        case '\n':
            ++vbe->cursor.y;
            vbe->cursor.x = 0;
            break;
        default:
            {
                uint16_t font_pos = c * FONT_HEIGHT;

                for (int y_font = 0; y_font < FONT_HEIGHT; ++y_font)
                {
                    for (int x_font = 0; x_font < FONT_WIDTH; ++x_font)
                    {
                        if (video_font[font_pos + y_font] & (1 << x_font))
                            vbe->buffer[buffer_pos + FONT_WIDTH - x_font] = 10;
                    }

                    vbe_flush_buffer(vbe, buffer_pos, FONT_WIDTH);
                    buffer_pos += video->xres * video->bpp / 8;
                }

                ++vbe->cursor.x;

                if (vbe->cursor.x == video->xres / FONT_WIDTH)
                {
                    ++vbe->cursor.y;
                    vbe->cursor.x = 0;
                }
            }
            break;
    }

    if (vbe->cursor.y == video->yres / FONT_HEIGHT)
        vbe_scroll(video);
}

static int vbe_write(struct video *video, struct req_rdwr *req, size_t *size)
{
    struct vbe *vbe = video->private;
    char *buffer = req->data;

    if (!vbe->buffer)
        return -1;

    for (size_t i = 0; i < req->size; ++i)
        vbe_put_char(video, buffer[i]);

    *size = req->size;

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

    vbe->cursor.x = 0;
    vbe->cursor.y = 0;
    vbe->buffer = NULL;
    vbe->bank = 0;

    video->private = vbe;

    video->write = NULL;
    video->xres_set = vbe_xres_set;
    video->yres_set = vbe_yres_set;
    video->bpp_set = vbe_bpp_set;
    video->enable = vbe_enable;
    video->disable = vbe_disable;
    video->write = vbe_write;

    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);

    return 0;
}

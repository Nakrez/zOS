#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/io.h>

#include <zos/video.h>

#include "fallback.h"

static int fallback_xres_set(struct video *video, int xres)
{
    (void)video;
    (void)xres;

    return 0;
}

static int fallback_yres_set(struct video *video, int yres)
{
    (void) video;
    (void) yres;

    return 0;
}

static int fallback_bpp_set(struct video *video, int bpp)
{
    (void) video;
    (void) bpp;

    return 0;
}

static int fallback_enable(struct video *video)
{
    struct fallback *fallback = video->private;

    memset(fallback->phy_data, 0, FALLBACK_SIZE);

    return 0;
}

static int fallback_disable(struct video *video)
{
    (void) video;

    return 0;
}

static void fallback_scroll(struct fallback *fallback)
{
    uint32_t line_size = FALLBACK_XRES * FALLBACK_BPP;

    memcpy(fallback->phy_data, (char *)fallback->phy_data + line_size,
           FALLBACK_SIZE - line_size);
    memset(fallback->phy_data + (FALLBACK_YRES - 1) * FALLBACK_XRES *
           FALLBACK_BPP, 0, line_size);

    --fallback->cursor.y;
}

static void fallback_put_char(struct fallback *fallback, char c)
{
    uint32_t pos;

    pos = fallback->cursor.y * FALLBACK_XRES * FALLBACK_BPP +
          fallback->cursor.x * FALLBACK_BPP;

    switch (c)
    {
        case '\n':
            ++fallback->cursor.y;
            fallback->cursor.x = 0;
            break;
        default:
            {
                char *data_addr = (char *)fallback->phy_data + pos;
                char *color_addr = (char *)fallback->phy_data + pos + 1;

                *data_addr = c;

                /* Light grey */
                *color_addr = 7;

                ++fallback->cursor.x;

                if (fallback->cursor.x == FALLBACK_XRES)
                {
                    ++fallback->cursor.y;
                    fallback->cursor.x = 0;
                }
            }
            break;
    }

    if (fallback->cursor.y == FALLBACK_YRES)
        fallback_scroll(fallback);
}

static int fallback_write(struct video *video, struct req_rdwr *req, size_t *size)
{
    struct fallback *fallback = video->private;
    char *buffer = req->data;

    for (size_t i = 0; i < req->size; ++i)
        fallback_put_char(fallback, buffer[i]);

    *size = req->size;

    return 0;
}

int fallback_initialize(struct video *video)
{
    struct fallback *fallback;

    if (!(fallback = malloc(sizeof (struct fallback))))
        return -1;

    if (!(fallback->phy_area = mmap_physical((void *)FALLBACK_ADDR,
                                             FALLBACK_AREA_SIZE)))
    {
        free(fallback);

        return -1;
    }

    fallback->phy_data = (char *)fallback->phy_area + FALLBACK_DATA_OFF;

    video->xres = FALLBACK_XRES;
    video->yres = FALLBACK_YRES;
    video->bpp = FALLBACK_BPP;

    fallback->cursor.x = 0;
    fallback->cursor.y = 0;

    video->private = fallback;

    video->write = NULL;
    video->xres_set = fallback_xres_set;
    video->yres_set = fallback_yres_set;
    video->bpp_set = fallback_bpp_set;
    video->enable = fallback_enable;
    video->disable = fallback_disable;
    video->write = fallback_write;

    return 0;
}

#include <zos/print.h>
#include <zos/video.h>

#include "vbe.h"
#include "fallback.h"

int video_initialize(struct video *video)
{
    uint16_t vbe_index = bga_read_register(VBE_DISPI_INDEX_ID);

    if (vbe_index < VBE_DIPSI_ID0 || vbe_index > VBE_DIPSI_ID5)
    {
        uprint("Video: VBE Extended version not supported");
        uprint("Video: Trying fallback mode");

        return fallback_initialize(video);
    }

    return vbe_initialize(video);
}

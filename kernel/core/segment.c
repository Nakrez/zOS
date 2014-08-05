#include <kernel/segment.h>

void segment_initialize(struct boot_info *boot)
{
    (void) boot;

    __segment.init();
}

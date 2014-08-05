#include <glue/segment.h>

#include <arch/gdt.h>
#include <arch/pm.h>

struct segment_glue __segment =
{
    segment_x86_pc_initialize,
};

void segment_x86_pc_initialize(void)
{
    gdt_init();
    pm_setup();
}

#include <kernel/as.h>
#include <kernel/zos.h>
#include <kernel/kmalloc.h>
#include <kernel/region.h>

struct as kernel_as;

struct as *as_create(void)
{
    struct as *as = kmalloc(sizeof (struct as));

    if (!as_initialize(as))
    {
        kfree(as);

        return NULL;
    }

    return as;
}

int as_initialize(struct as* as)
{
    if (!__as.init(as))
        return 0;

    region_initialize(as);

    return 1;
}

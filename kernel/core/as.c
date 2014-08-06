#include <kernel/as.h>
#include <kernel/zos.h>
#include <kernel/kmalloc.h>

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
    return __as.init(as);
}

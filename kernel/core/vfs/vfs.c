#include <kernel/vfs/vfs.h>
#include <kernel/vfs/path_tree.h>

#include <kernel/kmalloc.h>

int vfs_initialize(void)
{
    if (vtree_initialize())
        return -1;

    return 0;
}

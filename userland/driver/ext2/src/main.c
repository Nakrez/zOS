#include <zos/print.h>

#include "fs.h"

int main(void)
{
    if (!ext2fs_initialize("/dev/ata-disk0"))
    {
        uprint("EXT2: an error occured in initialization. Bye!");

        return 1;
    }

    uprint("EXT2: ready");

    return 0;
}

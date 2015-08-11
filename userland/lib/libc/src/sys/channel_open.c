#include <zos/vfs.h>

#include <arch/syscall.h>

int channel_open(const char *c_name)
{
    int ret;

    SYSCALL1(SYS_CHANNEL_OPEN, c_name, ret);

    return ret;
}

#include <unistd.h>

#include <arch/syscall.h>

int usleep(size_t mseconds)
{
    int ret;

    SYSCALL1(SYS_USLEEP, mseconds, ret);

    return ret;
}

#include <unistd.h>

#include <arch/syscall.h>

int fork(void)
{
    int ret;

    SYSCALL0(SYS_FORK, ret);

    return ret;
}

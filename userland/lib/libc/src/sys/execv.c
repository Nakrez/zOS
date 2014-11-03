#include <unistd.h>

#include <arch/syscall.h>

int execv(const char *filename, char *const argv[])
{
    int ret;

    SYSCALL2(SYS_EXECV, filename, argv, ret);

    return ret;
}

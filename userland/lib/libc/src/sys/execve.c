#include <unistd.h>

#include <arch/syscall.h>

int execve(const char *filename, char *const argv[], char *const envp[])
{
    int ret;

    SYSCALL3(SYS_EXECVE, filename, argv, envp, ret);

    return ret;
}

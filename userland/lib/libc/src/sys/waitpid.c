#include <sys/wait.h>

#include <arch/syscall.h>

pid_t waitpid(pid_t pid, int *status, int options)
{
    pid_t ret;

    SYSCALL3(SYS_WAITPID, pid, status, options, ret);

    return ret;
}

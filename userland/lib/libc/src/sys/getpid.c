#include <unistd.h>

#include <arch/syscall.h>

pid_t getpid(void)
{
    pid_t pid;

    SYSCALL0(SYS_GETPID, pid);

    return pid;
}

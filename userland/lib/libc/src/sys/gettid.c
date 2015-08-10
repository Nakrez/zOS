#include <unistd.h>

#include <arch/syscall.h>

pid_t gettid(void)
{
    pid_t tid;

    SYSCALL0(SYS_GETTID, tid);

    return tid;
}

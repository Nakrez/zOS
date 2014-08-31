#include <thread.h>

#include <arch/syscall.h>

void thread_exit(void)
{
    int ret;

    SYSCALL0(SYS_THREAD_EXIT, ret);
}

#include <kernel/syscall.h>
#include <kernel/thread.h>

int sys_gettid(struct syscall *interface)
{
    (void)interface;

    return thread_current()->tid;
}

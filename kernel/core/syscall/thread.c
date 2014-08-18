#include <kernel/syscall.h>
#include <kernel/thread.h>

int sys_thread_create(struct syscall *interface)
{
    return thread_create(thread_current()->parent, interface->arg1);
}

int sys_thread_exit(struct syscall *interface)
{
    (void)interface;

    thread_exit(thread_current());

    return 0;
}

int sys_gettid(struct syscall *interface)
{
    (void)interface;

    return thread_current()->tid;
}

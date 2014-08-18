#include <kernel/syscall.h>
#include <kernel/process.h>
#include <kernel/thread.h>

int sys_usleep(struct syscall *interface)
{
    thread_sleep(thread_current(), interface->arg1);

    return 0;
}

int sys_exit(struct syscall *interface)
{
    process_exit(thread_current()->parent, interface->arg1);

    return 0;
}

int sys_fork(struct syscall *interface)
{
    (void)interface;

    return process_fork(thread_current()->parent, interface->regs);
}

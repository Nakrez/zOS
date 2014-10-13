#include <kernel/syscall.h>

#include <kernel/proc/process.h>
#include <kernel/proc/thread.h>

int sys_usleep(struct syscall *interface)
{
    thread_sleep(thread_current(), interface->arg1, interface->regs);

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

int sys_getpid(struct syscall *interface)
{
    (void)interface;

    return thread_current()->parent->pid;
}

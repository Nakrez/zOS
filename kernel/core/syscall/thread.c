#include <kernel/syscall.h>
#include <kernel/cpu.h>

#include <kernel/proc/thread.h>

int sys_thread_create(struct syscall *interface)
{
    return thread_create(thread_current()->parent, interface->arg1,
                         2, interface->arg2, interface->arg3);
}

int sys_thread_exit(struct syscall *interface)
{
    struct cpu *cpu = cpu_get(cpu_id_get());

    (void)interface;

    thread_exit(thread_current());

    cpu->scheduler.time = 1;

    scheduler_update(NULL);

    return 0;
}

int sys_gettid(struct syscall *interface)
{
    (void)interface;

    return thread_current()->tid;
}

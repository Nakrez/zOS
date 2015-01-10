#include <kernel/syscall.h>
#include <kernel/cpu.h>

#include <kernel/proc/thread.h>

int sys_thread_create(struct syscall *interface)
{
    uintptr_t code = (uintptr_t)interface->arg1;
    int argc = (int)interface->arg2;
    char **argv = (void *)interface->arg3;

    return thread_create(thread_current()->parent, code, argc, argv, 0);
}

int sys_thread_exit(struct syscall *interface)
{
    struct cpu *cpu = cpu_get(cpu_id_get());

    (void)interface;

    thread_exit(thread_current());

    cpu->scheduler.time = 1;

    scheduler_update(NULL, 1);

    return 0;
}

int sys_gettid(struct syscall *interface)
{
    (void)interface;

    return thread_current()->tid;
}

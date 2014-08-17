#include <string.h>

#include <kernel/syscall.h>
#include <kernel/panic.h>
#include <kernel/console.h>
#include <kernel/thread.h>
#include <kernel/zos.h>

static int sys_print(struct syscall *interface)
{
    console_message(T_INF, "Syscall print: %s", (char *)interface->arg1);

    return 0;
}

static int sys_usleep(struct syscall *interface)
{
    thread_sleep(thread_current(), interface->arg1);

    return 0;
}

static int sys_exit(struct syscall *interface)
{
    process_exit(thread_current()->parent, interface->arg1);

    return 0;
}

static int sys_fork(struct syscall *interface)
{
    (void)interface;

    return process_fork(thread_current()->parent, interface->regs);
}

static syscall_callback syscalls[] =
{
    &sys_print,
    &sys_usleep,
    &sys_exit,
    &sys_fork,
};

void syscall_handler(struct irq_regs *regs)
{
    struct syscall call;

    /*
     * Convert irq registers (arch dependent) to independent syscall
     * interface
     */

    glue_call(syscall, convert, regs, &call);

    call.regs = regs;

    if (call.num <= 0 || call.num > SYSCALL_MAX)
    {
        *call.ret = -1;
        return;
    }

    *call.ret = syscalls[call.num - 1](&call);
}

void syscall_initialize(void)
{
    glue_call(syscall, init);
}

#include <string.h>

#include <kernel/syscall.h>
#include <kernel/panic.h>
#include <kernel/console.h>
#include <kernel/thread.h>

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

static syscall_callback syscalls[] =
{
    &sys_print,
    &sys_usleep,
};

void syscall_handler(struct irq_regs *regs)
{
    struct syscall call;

    /*
     * Convert irq registers (arch dependent) to independent syscall
     * interface
     */
    _syscall.convert(regs, &call);

    if (call.num <= 0 || call.num > SYSCALL_MAX)
    {
        *call.ret = -1;
        return;
    }

    *call.ret = syscalls[call.num - 1](&call);
}

void syscall_initialize(void)
{
    _syscall.init();
}

#include <string.h>

#include <kernel/syscall.h>
#include <kernel/panic.h>
#include <kernel/console.h>
#include <kernel/zos.h>

static int syscall_max;

static int sys_print(struct syscall *interface)
{
    console_message(T_INF, "Syscall print: %s", (char *)interface->arg1);

    return 0;
}

static syscall_callback syscalls[] =
{
    &sys_print,
    &sys_usleep,
    &sys_exit,
    &sys_fork,
    &sys_getpid,
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

    if (call.num <= 0 || call.num > syscall_max)
    {
        *call.ret = -1;
        return;
    }

    *call.ret = syscalls[call.num - 1](&call);
}

void syscall_initialize(void)
{
    syscall_max = sizeof (syscalls) / sizeof (syscall_callback);

    glue_call(syscall, init);
}

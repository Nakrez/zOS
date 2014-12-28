#include <string.h>

#include <kernel/zos.h>
#include <kernel/panic.h>
#include <kernel/console.h>
#include <kernel/syscall.h>

#include <kernel/proc/thread.h>

static int syscall_max;

static int sys_uprint(struct syscall *interface)
{
    console_message(T_INF, "Userland (%i, %i): %s",
                    thread_current()->parent->pid, thread_current()->tid,
                    (char *)interface->arg1);

    return 0;
}

static syscall_callback syscalls[] =
{
    &sys_uprint,

    /* Process */
    &sys_usleep,
    &sys_exit,
    &sys_fork,
    &sys_getpid,
    &sys_waitpid,
    &sys_execv,

    /* Thread */
    &sys_thread_create,
    &sys_thread_exit,
    &sys_gettid,

    /* Interrupts */
    &sys_interrupt_register,
    &sys_interrupt_listen,
    &sys_interrupt_unregister,

    /* Memory */
    sys_mmap,
    sys_munmap,
    sys_mmap_physical,

    /* Vfs */
    sys_vfs_device_create,
    sys_vfs_device_recv_request,
    sys_vfs_device_send_response,

    sys_vfs_open,
    sys_vfs_read,
    sys_vfs_write,
    sys_vfs_close,
    sys_vfs_lseek,

    sys_vfs_mount,

    sys_vfs_stat,
    sys_vfs_fstat,

    sys_vfs_ioctl,

    sys_vfs_dup,
    sys_vfs_dup2,

    sys_vfs_getdirent,
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

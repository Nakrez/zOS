#include <boot/boot.h>

#include <kernel/config.h>
#include <kernel/panic.h>
#include <kernel/console.h>
#include <kernel/interrupt.h>
#include <kernel/timer.h>
#include <kernel/syscall.h>
#include <kernel/module.h>
#include <kernel/cpu.h>

#include <kernel/mem/segment.h>
#include <kernel/mem/as.h>
#include <kernel/mem/kmalloc.h>

#include <kernel/proc/process.h>

#include <kernel/vfs/vfs.h>

void kernel_main(struct boot_info *boot)
{
    (void)boot;

#ifdef CONFIG_CONSOLE
    console_init();

    console_message(T_INF, "zKernel is booting");

# ifdef CONFIG_MEMORY
    kmalloc_initialize(boot);

    segment_initialize(boot);

    if (!as_initialize(&kernel_as))
        kernel_panic("Fail to initialize kernel address space");

    console_message(T_OK, "Kernel address space initialized");

#  ifdef CONFIG_INTERRUPT
    interrupt_initialize();

    console_message(T_OK, "Interrupt initialized");
#  endif /* CONFIG_INTERRUPT */

#  ifdef CONFIG_TIMER
    timer_initialize();

    console_message(T_OK, "Timer initialized");
#  endif /* CONFIG_TIMER */

#  ifdef CONFIG_VFS
    vfs_initialize();

    console_message(T_OK, "VFS initialized");
#  endif /* CONFIG_VFS */

#  ifdef CONFIG_PROCESS
    process_initialize();

    console_message(T_OK, "Process initialized");
#  endif /* CONFIG_PROCESS */

#  ifdef CONFIG_SYSCALL
    syscall_initialize();

    console_message(T_OK, "System call initialized");
#  endif /* CONFIG_SYSCALL */

#  ifdef CONFIG_SCHEDULER
    cpu_initialize();
#  endif /* CONFIG_SCHEDULER */

#  ifdef CONFIG_MODULE
    module_initialize(boot);
#  endif /* CONFIG_MODULE */

#  ifdef CONFIG_SCHEDULER
    cpu_start();
#  endif /* CONFIG_SCHEDULER */
# endif /* CONFIG_MEMORY */

    kernel_panic("Unreachable code reached");
#endif /* CONFIG_CONSOLE */
}

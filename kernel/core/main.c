#include <boot/boot.h>

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

    console_init();

    console_message(T_INF, "zKernel is booting");

    kmalloc_initialize(boot);

    segment_initialize(boot);

    if (!as_initialize(&kernel_as))
        kernel_panic("Fail to initialize kernel address space");

    console_message(T_OK, "Kernel address space initialized");

    interrupt_initialize();

    console_message(T_OK, "Interrupt initialized");

    timer_initialize();

    console_message(T_OK, "Timer initialized");

    vfs_initialize();

    console_message(T_OK, "VFS initialized");

    process_initialize();

    console_message(T_OK, "Process initialized");

    syscall_initialize();

    console_message(T_OK, "System call initialized");

    cpu_initialize();

    module_initialize(boot);

    cpu_start();

    kernel_panic("Unreachable code reached");
}

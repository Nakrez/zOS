#include <boot/boot.h>

#include <kernel/console.h>
#include <kernel/kmalloc.h>
#include <kernel/segment.h>
#include <kernel/as.h>
#include <kernel/event.h>
#include <kernel/timer.h>
#include <kernel/process.h>
#include <kernel/syscall.h>
#include <kernel/module.h>
#include <kernel/cpu.h>
#include <kernel/panic.h>

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

    event_initialize();

    console_message(T_OK, "Event initialized");

    timer_initialize();

    console_message(T_OK, "Timer initialized");

    process_initialize();

    console_message(T_OK, "Process initialized");

    syscall_initialize();

    console_message(T_OK, "System call initialized");

    cpu_initialize();

    module_initialize(boot);

    cpu_start();

    kernel_panic("Unreachable code reached");
}

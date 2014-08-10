#include <kernel/panic.h>

#include <glue/syscall.h>

#include <arch/idt.h>

struct glue_syscall _syscall =
{
    i386_pc_syscall_initialize,
    i386_pc_syscall_convert,
};

void i386_pc_syscall_initialize(void)
{
    if (!event_register(IRQ_SYSCALL, EVENT_CALLBACK, syscall_handler))
        kernel_panic("Unable to register syscall event");
}

void i386_pc_syscall_convert(struct irq_regs *regs, struct syscall *call)
{
    call->num = regs->eax;

    call->ret = (int *)&regs->eax;

    call->arg1 = regs->ebx;
    call->arg2 = regs->ecx;
    call->arg3 = regs->edx;
    call->arg4 = regs->esi;
    call->arg5 = regs->edi;
}

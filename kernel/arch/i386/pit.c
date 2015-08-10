#include <kernel/types.h>
#include <kernel/time.h>
#include <kernel/panic.h>

#include <arch/pit.h>
#include <arch/io.h>

void pit_initialize(void)
{
    uint32_t pit_value = PIT_RATE / TICK_PER_SEC;

    if (pit_value == 0 || pit_value > PIT_VALUE_MAX)
        kernel_panic("Fail to calculate correct value for the PIT");

    outb(PIT_CMD, 0x34);

    outb(PIT0_DATA, pit_value & 0xFF);
    outb(PIT0_DATA, (pit_value >> 8) & 0xFF);
}

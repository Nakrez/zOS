#include <kernel/types.h>
#include <kernel/console.h>

void arch_back_trace(void)
{
    uint32_t ebp;

    __asm__ __volatile__("mov %%ebp, %0"
                         : "=r" (ebp)
                         :);

    while (ebp)
    {
        uint32_t eip_stack = ebp + 4;

        if (eip_stack < 0xC0000000)
            break;

        uint32_t eip = *(uint32_t *)eip_stack;

        if (eip < 0xC0000000)
            break;

        console_message(T_INF, "0x%x", eip);

        ebp = *(uint32_t *)ebp;
    }
}

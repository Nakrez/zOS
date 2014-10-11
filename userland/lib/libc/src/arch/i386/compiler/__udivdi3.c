#include <string.h>

#include <arch/compiler.h>

__libgcc uint64_t __udivdi3(uint64_t num, uint64_t den)
{
    return __udivmoddi4(num, den, NULL);
}

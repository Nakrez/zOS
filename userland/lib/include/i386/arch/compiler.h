#ifndef I386_COMPILER_H
# define I386_COMPILER_H

# include <stdint.h>

# define __libgcc __attribute__ ((cdecl))

__libgcc uint64_t __udivmoddi4(uint64_t num, uint64_t den, uint32_t *rem_p);
__libgcc uint64_t __udivdi3(uint64_t num, uint64_t den);

#endif /* !I386_COMPILER_H */

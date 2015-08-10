#ifndef X86_REGS_H
# define X86_REGS_H

# include <kernel/types.h>

static inline void cs_set(uint32_t cs)
{
    __asm__ __volatile__("pushl %0\n"
                         "pushl $1f\n"
                         "lret\n"
                         "1:"
                         :
                         : "r" (cs)
                         : "memory");
}

static inline uint32_t eflags_get(void)
{
    uint32_t ret;

    __asm__ __volatile__("pushfl\n"
                         "pop %%eax\n"
                         "mov %%eax, %0\n"
                         : "=r" (ret)
                         :);

    return ret;
}

static inline void eflags_set(uint32_t eflags)
{
    __asm__ __volatile__("pushl %0\n"
                         "popfl\n"
                         :
                         : "g" (eflags)
                         : "memory");
}

# define REG_SET(name)                              \
    static inline void name ## _set(uint16_t name)  \
    {                                               \
        __asm__ __volatile__("mov %0, %%" #name     \
                             :                      \
                             : "r" (name)           \
                             : "memory");           \
    }

REG_SET(ds);
REG_SET(es);
REG_SET(fs);
REG_SET(gs);
REG_SET(ss);

#endif /* !X86_REGS_H */

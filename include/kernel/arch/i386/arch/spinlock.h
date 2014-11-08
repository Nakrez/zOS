#ifndef ARCH_I386_SPINLOCK_H
# define ARCH_I386_SPINLOCK_H

# include <arch/cpu.h>

# define SPINLOCK_INIT { 0, 0 }

typedef struct
{
    int lock;
    uint32_t eflags;
} spinlock_t;

static inline void spinlock_init(spinlock_t *spin)
{
    spin->lock = 0;
    spin->eflags = 0;
}

static inline void spinlock_lock(spinlock_t *spin)
{
    uint32_t eflags = eflags_get();

    __asm__ __volatile__ ("cli\n"
                          "1:\n"
                          "mov $0x1, %%eax\n"
                          "lock xchg (%0), %%eax\n"
                          "test %%eax, %%eax\n"
                          "jnz 1b\n"
                          :
                          : "b" (&spin->lock)
                          : "memory", "eax");

    spin->eflags = eflags;
}

static inline void spinlock_unlock_no_restore(spinlock_t *spin)
{
    __asm__ __volatile__ ("mov $0x0, %%eax\n"
                          "lock xchg (%0), %%eax\n"
                          :
                          : "b" (&spin->lock)
                          : "memory", "eax");
}

static inline void spinlock_unlock(spinlock_t *spin)
{
    spinlock_unlock_no_restore(spin);

    eflags_set(spin->eflags);
}

#endif /* !ARCH_I386_SPINLOCK_H */

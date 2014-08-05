#ifndef ARCH_I386_SPINLOCK_H
# define ARCH_I386_SPINLOCK_H

# define SPINLOCK_INIT 0

typedef int spinlock_t;

static inline void spinlock_init(spinlock_t *spin)
{
    *spin = SPINLOCK_INIT;
}

static inline void spinlock_lock(spinlock_t *spin)
{
    __asm__ __volatile__ ("1:\n"
                          "mov $0x1, %%eax\n"
                          "lock xchg (%0), %%eax\n"
                          "test %%eax, %%eax\n"
                          "jnz 1b\n"
                          :
                          : "b" (spin));
}

static inline void spinlock_unlock(spinlock_t *spin)
{
    __asm__ __volatile__ ("mov $0x0, %%eax\n"
                          "lock xchg (%0), %%eax\n"
                          :
                          : "b" (spin));
}

#endif /* !ARCH_I386_SPINLOCK_H */

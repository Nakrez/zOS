#ifndef I386_SYS_IO_H
# define I386_SYS_IO_H

# include <stdint.h>

static inline void outb(uint16_t port, uint8_t data)
{
    __asm__ __volatile__("outb %b0, %w1"
                         :
                         : "a" (data)
                         , "Nd" (port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t data;

    __asm__ __volatile__("inb %w1, %b0"
                         : "=a" (data)
                         : "Nd" (port));

    return data;
}

static inline uint16_t inw(uint16_t port)
{
    uint16_t data;

    __asm__ __volatile__("inw %w1, %w0"
                         : "=a" (data)
                         : "Nd" (port));

    return data;
}

static inline void outl(uint16_t port, uint32_t data)
{
    __asm__ __volatile__("outl %0, %w1"
                         :
                         : "a" (data)
                         , "Nd" (port));
}

static inline uint32_t inl(uint16_t port)
{
    uint32_t data;

    __asm__ __volatile__("inl %w1, %0"
                         : "=a" (data)
                         : "Nd" (port));

    return data;
}

#endif /* !I386_SYS_IO_H */

#ifndef X86_IO_H
# define X86_IO_H

# include <kernel/types.h>

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

#endif /* !X86_IO_H */

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

#endif /* !X86_IO_H */

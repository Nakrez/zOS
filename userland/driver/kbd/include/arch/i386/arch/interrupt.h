#ifndef ARCH_I386_INTERRUPT_H
# define ARCH_I386_INTERRUPT_H

# define KEYBOARD_INTERRUPT 0x21

# define KEYBOARD_CMD 0x64
# define KEYBOARD_DATA 0x60

# define KEYBOARD_ACK 0xFA

void interrupt_thread(int argc, void *argv[]);

#endif /* !ARCH_I386_INTERRUPT_H */

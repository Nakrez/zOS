#ifndef LIBC_I386_SYSCALL_H
# define LIBC_I386_SYSCALL_H

# define SYS_USLEEP 2

# define SYSCALL1(num, arg1, ret)           \
    __asm__ __volatile__("mov %2, %%ebx\n"  \
                         "mov %1, %%eax\n"  \
                         "int $0x80\n"      \
                         "mov %%eax, %0\n"  \
                         : "=r" (ret)       \
                         : "i" (num),       \
                           "g" (arg1)       \
                         : "memory");

#endif /* !LIBC_I386_SYSCALL_H */
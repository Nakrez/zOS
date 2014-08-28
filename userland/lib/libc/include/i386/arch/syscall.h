#ifndef LIBC_I386_SYSCALL_H
# define LIBC_I386_SYSCALL_H

# define SYS_UPRINT 1
# define SYS_USLEEP 2
# define SYS_EXIT 3
# define SYS_FORK 4
# define SYS_GETPID 5
# define SYS_THREAD_CREATE 6
# define SYS_THREAD_EXIT 7
# define SYS_GETTID 8
# define SYS_INTERRUPT_REGISTER 9
# define SYS_INTERRUPT_LISTEN 10
# define SYS_INTERRUPT_UNREGISTER 11
# define SYS_DEVICE_CREATE 12

# define SYSCALL0(num, ret)                         \
    __asm__ __volatile__("mov %1, %%eax\n"          \
                         "int $0x80\n"              \
                         "mov %%eax, %0\n"          \
                         : "=r" (ret)               \
                         : "i" (num)                \
                         : "memory");

# define SYSCALL1(num, arg1, ret)                   \
    __asm__ __volatile__("mov %2, %%ebx\n"          \
                         "mov %1, %%eax\n"          \
                         "int $0x80\n"              \
                         "mov %%eax, %0\n"          \
                         : "=r" (ret)               \
                         : "i" (num),               \
                           "g" (arg1)               \
                         : "memory");

# define SYSCALL3(num, arg1, arg2, arg3, ret)       \
    __asm__ __volatile__("mov %4, %%edx\n"          \
                         "mov %3, %%ecx\n"          \
                         "mov %2, %%ebx\n"          \
                         "mov %1, %%eax\n"          \
                         "int $0x80\n"              \
                         "mov %%eax, %0\n"          \
                         : "=r" (ret)               \
                         : "i" (num),               \
                           "g" (arg1),              \
                           "g" (arg2),              \
                           "g" (arg3)               \
                         : "memory");

# define SYSCALL4(num, arg1, arg2, arg3, arg4, ret) \
    __asm__ __volatile__("mov %5, %%esi\n"          \
                         "mov %4, %%edx\n"          \
                         "mov %3, %%ecx\n"          \
                         "mov %2, %%ebx\n"          \
                         "mov %1, %%eax\n"          \
                         "int $0x80\n"              \
                         "mov %%eax, %0\n"          \
                         : "=r" (ret)               \
                         : "i" (num),               \
                           "g" (arg1),              \
                           "g" (arg2),              \
                           "g" (arg3),              \
                           "g" (arg4)               \
                         : "memory");

#endif /* !LIBC_I386_SYSCALL_H */

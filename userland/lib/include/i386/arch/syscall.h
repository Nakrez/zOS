#ifndef LIBC_I386_SYSCALL_H
# define LIBC_I386_SYSCALL_H

# define SYS_UPRINT 1
# define SYS_USLEEP 2
# define SYS_EXIT 3
# define SYS_FORK 4
# define SYS_GETPID 5
# define SYS_WAITPID 6
# define SYS_EXECV 7
# define SYS_THREAD_CREATE 8
# define SYS_THREAD_EXIT 9
# define SYS_GETTID 10
# define SYS_INTERRUPT_REGISTER 11
# define SYS_INTERRUPT_LISTEN 12
# define SYS_INTERRUPT_UNREGISTER 13
# define SYS_MMAP 14
# define SYS_MUNMAP 15
# define SYS_MMAP_PHYSICAL 16
# define SYS_DEVICE_CREATE 17
# define SYS_DEVICE_RECV_REQUEST 18
# define SYS_DEVICE_SEND_RESPONSE 19
# define SYS_OPEN 20
# define SYS_READ 21
# define SYS_WRITE 22
# define SYS_CLOSE 23
# define SYS_LSEEK 24
# define SYS_MOUNT 25
# define SYS_STAT 26
# define SYS_FSTAT 27
# define SYS_IOCTL 28
# define SYS_DUP 29
# define SYS_DUP2 30
# define SYS_GETDIRENT 31
# define SYS_DEVICE_EXISTS 32
# define SYS_OPEN_DEVICE 33
# define SYS_CHANNEL_CREATE 34
# define SYS_CHANNEL_OPEN 35

# define SYSCALL0(num, ret)                                 \
    __asm__ __volatile__("mov %1, %%eax\n"                  \
                         "int $0x80\n"                      \
                         "mov %%eax, %0\n"                  \
                         : "=r" (ret)                       \
                         : "i" (num)                        \
                         : "memory");

# define SYSCALL1(num, arg1, ret)                           \
    __asm__ __volatile__("mov %2, %%ebx\n"                  \
                         "mov %1, %%eax\n"                  \
                         "int $0x80\n"                      \
                         "mov %%eax, %0\n"                  \
                         : "=r" (ret)                       \
                         : "i" (num),                       \
                           "g" (arg1)                       \
                         : "memory", "ebx");

# define SYSCALL2(num, arg1, arg2, ret)                     \
    __asm__ __volatile__("mov %3, %%ecx\n"                  \
                         "mov %2, %%ebx\n"                  \
                         "mov %1, %%eax\n"                  \
                         "int $0x80\n"                      \
                         "mov %%eax, %0\n"                  \
                         : "=r" (ret)                       \
                         : "i" (num),                       \
                           "g" (arg1),                      \
                           "g" (arg2)                       \
                         : "memory", "ecx", "ebx");

# define SYSCALL3(num, arg1, arg2, arg3, ret)               \
    __asm__ __volatile__("mov %4, %%edx\n"                  \
                         "mov %3, %%ecx\n"                  \
                         "mov %2, %%ebx\n"                  \
                         "mov %1, %%eax\n"                  \
                         "int $0x80\n"                      \
                         "mov %%eax, %0\n"                  \
                         : "=r" (ret)                       \
                         : "i" (num),                       \
                           "g" (arg1),                      \
                           "g" (arg2),                      \
                           "g" (arg3)                       \
                         : "memory", "edx", "ecx", "ebx");

# define SYSCALL4(num, arg1, arg2, arg3, arg4, ret)         \
    __asm__ __volatile__("mov %5, %%esi\n"                  \
                         "mov %4, %%edx\n"                  \
                         "mov %3, %%ecx\n"                  \
                         "mov %2, %%ebx\n"                  \
                         "mov %1, %%eax\n"                  \
                         "int $0x80\n"                      \
                         "mov %%eax, %0\n"                  \
                         : "=r" (ret)                       \
                         : "i" (num),                       \
                           "g" (arg1),                      \
                           "g" (arg2),                      \
                           "g" (arg3),                      \
                           "g" (arg4)                       \
                         : "memory", "esi", "edx", "ecx", "ebx");

# define SYSCALL5(num, arg1, arg2, arg3, arg4, arg5, ret)   \
    __asm__ __volatile__("mov %6, %%edi\n"                  \
                         "mov %5, %%esi\n"                  \
                         "mov %4, %%edx\n"                  \
                         "mov %3, %%ecx\n"                  \
                         "mov %2, %%ebx\n"                  \
                         "mov %1, %%eax\n"                  \
                         "int $0x80\n"                      \
                         "mov %%eax, %0\n"                  \
                         : "=r" (ret)                       \
                         : "i" (num),                       \
                           "g" (arg1),                      \
                           "g" (arg2),                      \
                           "g" (arg3),                      \
                           "g" (arg4),                      \
                           "g" (arg5)                       \
                         : "memory", "edi", "esi", "edx", "ecx", "ebx");

#endif /* !LIBC_I386_SYSCALL_H */

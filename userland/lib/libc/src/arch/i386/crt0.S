.global _start
.global main
_start:
    call __libc_init

    call main

    push %eax
    call exit

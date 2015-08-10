.global _start
.global main
_start:
    call __libc_init
    push %eax
    cmp $0, %eax
    movl $0x1, %eax
    jl 1f
    pop %eax

    call main

1:
    push %eax
    call exit

.global _start
.global main
_start:
    call main

    push %eax
    call exit

#define ISR_ERROR(num)      \
.global isr_ ## num;        \
isr_ ## num:                \
    pushl   $num;           \
    jmp     isr_main;

# define ISR_NO_ERROR(num)  \
.global isr_ ## num;        \
isr_ ## num:                \
    pushl   $0;             \
    pushl   $num;           \
    jmp     isr_main;

isr_main:
    pushal
    push    %ds
    push    %es
    push    %fs
    push    %gs

    /* FIXME: There is a macro for it */
    push    %ebx
    mov     $0x10,  %bx
    mov     %bx,    %ds
    pop     %ebx

    push    %esp
    call    event_dispatch
    add     $4, %esp

    pop     %gs
    pop     %fs
    pop     %es
    pop     %ds
    popal
    add     $8,     %esp
    iret

#include "isr.def"

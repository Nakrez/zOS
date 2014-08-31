#include <arch/idt.h>

/* Declaration of isr prototype */
#define ISR_ERROR(num) \
    void isr_ ## num(void);
#define ISR_NO_ERROR(num) \
    void isr_ ## num(void);

#include "isr.def"

#undef ISR_ERROR
#undef ISR_NO_ERROR

struct idt_entry idt_entries[MAX_IRQ_NUMBER];
struct idt_ptr iptr;

void idt_add_entry(uint8_t num, uint32_t address, uint16_t type)
{
    struct idt_entry *entry = &idt_entries[num];

    entry->offsetl = address & 0xFFFF;
    entry->offseth = (address >> 16) & 0xFFFF;
    entry->select = 0x8;
    entry->type = type;
}

void idt_initialize(void)
{
    /* Add each isr in the IDT */
#define ISR_ERROR(num) \
    idt_add_entry(num, (uint32_t)isr_ ## num, INTERRUPT_GATE);
#define ISR_NO_ERROR(num) \
    idt_add_entry(num, (uint32_t)isr_ ## num, INTERRUPT_GATE);

#include "isr.def"

    /* "Re-add" syscall isr to change it type to TRAP_GATE */
    idt_add_entry(IRQ_SYSCALL, (uint32_t)isr_128, TRAP_GATE);


    iptr.size = sizeof (idt_entries) - 1;
    iptr.ptr = (uint32_t)idt_entries;

    __asm__ __volatile__("lidt %0\n"
                         :
                         : "m" (iptr)
                         : "memory");
}

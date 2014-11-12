CURDIR := kernel/arch/i386

OBJ-y := console.o back_trace.o gdt.o pm.o mmu.o idt.o isr.o pic.o pit.o \
		thread.o scheduler.o tss.o page_fault.o

BINSUBDIRS-y := debug

include $(SRCDIR)/mk/binsubdirs.mk

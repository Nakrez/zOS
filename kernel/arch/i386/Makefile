CURDIR := kernel/arch/i386

OBJ-y :=
OBJ-$(CONFIG_CONSOLE) += console.o
OBJ-$(CONFIG_PANIC) += back_trace.o
OBJ-$(CONFIG_MEMORY) += gdt.o pm.o mmu.o page_fault.o
OBJ-$(CONFIG_INTERRUPT) += idt.o isr.o pic.o
OBJ-$(CONFIG_TIMER) += pit.o
OBJ-$(CONFIG_PROCESS) += thread.o tss.o
OBJ-$(CONFIG_SCHEDULER) += scheduler.o

BINSUBDIRS-y := debug

include $(SRCDIR)/mk/binsubdirs.mk

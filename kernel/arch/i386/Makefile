CURDIR := kernel/arch/i386/

OBJ := console.o back_trace.o gdt.o pm.o mmu.o idt.o isr.o pic.o pit.o \
		thread.o scheduler.o tss.o page_fault.o

DEPS := $(DEPS) $(addprefix $(CURDIR), $(OBJ:.o=.d))

$(BIN)_OBJ += $(addprefix $(DESTDIR), $(addprefix $(CURDIR), $(OBJ)))

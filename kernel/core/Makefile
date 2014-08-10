CURDIR := kernel/core/

OBJ := main.o string.o console.o kmalloc.o segment.o panic.o as.o region.o \
		event.o timer.o process.o thread.o cpu.o scheduler.o syscall.o

DEPS := $(DEPS) $(addprefix $(CURDIR), $(OBJ:.o=.d))

$(BIN)_OBJ += $(addprefix $(DESTDIR), $(addprefix $(CURDIR), $(OBJ)))

CURDIR := kernel/core

OBJ-y := main.o string.o console.o panic.o interrupt.o timer.o cpu.o syscall.o \
		module.o

BINSUBDIRS-y := mem proc scheduler vfs syscall

include $(SRCDIR)/mk/binsubdirs.mk

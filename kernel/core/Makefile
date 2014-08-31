CURDIR := kernel/core/

OBJ := main.o string.o console.o kmalloc.o segment.o panic.o as.o region.o \
		interrupt.o timer.o process.o thread.o cpu.o syscall.o \
		module.o

BINSUBDIRS := syscall/ vfs/ scheduler/

include ${SRCDIR}mk/binsubdirs.mk

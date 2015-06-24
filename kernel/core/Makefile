CURDIR := kernel/core

OBJ-y := main.o string.o

OBJ-$(CONFIG_CONSOLE) += console.o
OBJ-$(CONFIG_PANIC) += panic.o
OBJ-$(CONFIG_INTERRUPT) += interrupt.o
OBJ-$(CONFIG_TIMER) += time.o
OBJ-$(CONFIG_TIMER) += timer.o
OBJ-$(CONFIG_SCHEDULER) += cpu.o
OBJ-$(CONFIG_SYSCALL) += syscall.o
OBJ-$(CONFIG_MODULE) += module.o

BINSUBDIRS-y :=
BINSUBDIRS-$(CONFIG_MEMORY) += mem
BINSUBDIRS-$(CONFIG_PROCESS) += proc
BINSUBDIRS-$(CONFIG_SCHEDULER) += scheduler
BINSUBDIRS-$(CONFIG_VFS) += vfs
BINSUBDIRS-$(CONFIG_SYSCALL) += syscall

include $(SRCDIR)/mk/binsubdirs.mk

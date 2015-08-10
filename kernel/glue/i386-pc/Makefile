CURDIR := kernel/glue/i386-pc

OBJ-y :=
OBJ-$(CONFIG_CONSOLE) += console.o
OBJ-$(CONFIG_MEMORY) += segment.o as.o
OBJ-$(CONFIG_INTERRUPT) += interrupt.o
OBJ-$(CONFIG_TIMER) += timer.o
OBJ-$(CONFIG_PROCESS) += thread.o
OBJ-$(CONFIG_SCHEDULER) += scheduler.o cpu.o
OBJ-$(CONFIG_SYSCALL) += syscall.o

include $(SRCDIR)/mk/binsubdirs.mk

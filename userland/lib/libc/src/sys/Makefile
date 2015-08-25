CURDIR := userland/lib/libc/src/sys

OBJ-y := usleep.o sleep.o exit.o fork.o waitpid.o getpid.o gettid.o \
		thread_exit.o thread_create.o interrupt_register.o \
		interrupt_listen.o interrupt_unregister.o uprint.o \
		device_create.o open.o read.o write.o close.o lseek.o mmap.o munmap.o \
		mount.o stat.o fstat.o execv.o ioctl.o mmap_physical.o dup.o \
		dup2.o getdirent.o device_exists.o open_device.o channel_create.o \
		channel_open.o fs_register.o fs_unregister.o

LIBSUBDIRS-y :=

include $(SRCDIR)/mk/libsubdirs.mk

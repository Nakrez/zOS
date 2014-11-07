CURDIR := userland/lib/libc/src/sys/

OBJ := usleep.o sleep.o exit.o fork.o waitpid.o getpid.o gettid.o \
		thread_exit.o thread_create.o interrupt_register.o interrupt_listen.o \
		interrupt_unregister.o uprint.o device_create.o device_recv_request.o \
		device_send_response.o open.o read.o write.o close.o lseek.o mmap.o \
		munmap.o mount.o stat.o fstat.o execv.o ioctl.o mmap_physical.o dup.o \
		dup2.o

LIBSUBDIRS :=

include $(SRCDIR)mk/libsubdirs.mk

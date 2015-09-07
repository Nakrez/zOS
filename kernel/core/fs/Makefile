CURDIR := kernel/core/fs

OBJ-y := fiu.o channel.o
OBJ-$(CONFIG_DEVFS) += devfs.o

BINSUBDIRS-y := vfs ops

include $(SRCDIR)/mk/binsubdirs.mk

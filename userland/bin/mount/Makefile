CURDIR := userland/bin/mount

BIN-y :=
BIN-$(CONFIG_BUILD_MOUNT) := mount

BINSUBDIRS-y :=

INSTALL_DIR := bin

mount_CFLAGS := $(USERLAND_CFLAGS)
mount_LDFLAGS := $(USERLAND_LDFLAGS)

mount_LIBS := libc

OBJ-y := mount.o

include $(SRCDIR)/mk/bin.mk


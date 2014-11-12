CURDIR := userland/driver/ext2

BIN-y :=
BIN-$(CONFIG_BUILD_EXT2) := ext2

BINSUBDIRS-y :=
BINSUBDIRS-$(CONFIG_BUILD_EXT2) := src

INSTALL_DIR := bin

ext2_CFLAGS := $(USERLAND_CFLAGS)
ext2_LDFLAGS := $(USERLAND_LDFLAGS)

ext2_LIBS := libfiu libc

OBJ-y :=

include $(SRCDIR)/mk/bin.mk

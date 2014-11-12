CURDIR := userland/driver/ext2

BIN-y := ext2
INSTALL_DIR := bin
BINSUBDIRS-y := src

ext2_CFLAGS := $(USERLAND_CFLAGS)
ext2_LDFLAGS := $(USERLAND_LDFLAGS)

ext2_LIBS := libfiu libc

OBJ-y :=

include $(SRCDIR)/mk/bin.mk

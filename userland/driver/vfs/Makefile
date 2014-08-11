CURDIR := userland/driver/vfs/

BIN := vfs
INSTALL_DIR := bin
BINSUBDIRS := src/

vfs_CFLAGS := -I${SRCDIR}userland/lib/libc/include
vfs_CFLAGS := -std=c99 -nostdinc -fno-builtin -fno-common -Wall -Wextra

vfs_LDFLAGS := $(USERLAND_LDFLAGS)

vfs_LIBS := libc

OBJ :=

include ${SRCDIR}mk/bin.mk


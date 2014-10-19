CURDIR := userland/driver/ext2/

BIN := ext2
INSTALL_DIR := bin
BINSUBDIRS := src/

ext2_CFLAGS := -I${SRCDIR}userland/lib/libc/include
ext2_CFLAGS += -I${SRCDIR}userland/lib/libc/include/${ZOS_ARCH}/
ext2_CFLAGS += -I${SRCDIR}userland/lib/libfiu/include/
ext2_CFLAGS += $(USERLAND_CFLAGS)

ext2_LDFLAGS := $(USERLAND_LDFLAGS)

ext2_LIBS := libfiu libc

OBJ :=

include ${SRCDIR}mk/bin.mk

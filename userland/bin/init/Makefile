CURDIR := userland/bin/init/

BIN := init
INSTALL_DIR := bin
BINSUBDIRS := src/

init_CFLAGS := -I${SRCDIR}userland/lib/libc/include
init_CFLAGS += -I${SRCDIR}userland/lib/libc/include/${ZOS_ARCH}/
init_CFLAGS += $(USERLAND_CFLAGS)

init_LDFLAGS := $(USERLAND_LDFLAGS)

init_LIBS := libc

OBJ :=

include ${SRCDIR}mk/bin.mk

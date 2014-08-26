CURDIR := userland/driver/kbd/

BIN := kbd
INSTALL_DIR := bin
BINSUBDIRS := src/

kbd_CFLAGS := -I${SRCDIR}userland/lib/libc/include
kbd_CFLAGS += -I${SRCDIR}userland/lib/libc/include/${ZOS_ARCH}/
kbd_CFLAGS += -I${SRCDIR}userland/driver/kbd/include/
kbd_CFLAGS += -std=c99 -nostdinc -fno-builtin -fno-common -Wall -Wextra

kbd_LDFLAGS := $(USERLAND_LDFLAGS)

kbd_LIBS := libc

OBJ :=

include ${SRCDIR}mk/bin.mk


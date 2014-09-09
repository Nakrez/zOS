CURDIR := userland/driver/i386/ata/

BIN := ata
INSTALL_DIR := bin
BINSUBDIRS := src/

ata_CFLAGS := -I${SRCDIR}userland/lib/libc/include
ata_CFLAGS += -I${SRCDIR}userland/lib/libc/include/${ZOS_ARCH}/
ata_CFLAGS += -I${SRCDIR}userland/lib/libdriver/include
ata_CFLAGS += -std=c99 -nostdinc -fno-builtin -fno-common -Wall -Wextra

ata_LDFLAGS := $(USERLAND_LDFLAGS)

ata_LIBS := libdriver libc

OBJ :=

include ${SRCDIR}mk/bin.mk


CURDIR := userland/driver/i386/ata/

BIN := ata
INSTALL_DIR := bin
BINSUBDIRS := src/

ata_CFLAGS := $(USERLAND_CFLAGS)
ata_LDFLAGS := $(USERLAND_LDFLAGS)

ata_LIBS := libdriver libc

OBJ :=

include ${SRCDIR}mk/bin.mk

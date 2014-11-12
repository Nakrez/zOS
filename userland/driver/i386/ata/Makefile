CURDIR := userland/driver/i386/ata

BIN-y := ata
INSTALL_DIR := bin
BINSUBDIRS-y := src

ata_CFLAGS := $(USERLAND_CFLAGS)
ata_LDFLAGS := $(USERLAND_LDFLAGS)

ata_LIBS := libdriver libc

OBJ-y :=

include $(SRCDIR)/mk/bin.mk

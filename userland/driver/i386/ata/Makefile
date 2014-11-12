CURDIR := userland/driver/i386/ata

BIN-y :=
BIN-$(CONFIG_ATA) := ata

BINSUBDIRS-y :=
BINSUBDIRS-$(CONFIG_ATA) := src

INSTALL_DIR := bin

ata_CFLAGS := $(USERLAND_CFLAGS)
ata_LDFLAGS := $(USERLAND_LDFLAGS)

ata_LIBS := libdriver libc

OBJ-y :=

include $(SRCDIR)/mk/bin.mk

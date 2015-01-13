CURDIR := userland/driver/ramd

BIN-y :=
BIN-$(CONFIG_BUILD_RAMD) := ramd

BINSUBDIRS-y :=

INSTALL_DIR := bin

ramd_CFLAGS := $(USERLAND_CFLAGS)
ramd_LDFLAGS := $(USERLAND_LDFLAGS)

ramd_LIBS := libdriver libc

OBJ-y := ramd.o

include $(SRCDIR)/mk/bin.mk

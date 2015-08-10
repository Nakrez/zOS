CURDIR := userland/bin/init

BIN-y :=
BIN-$(CONFIG_BUILD_INIT) := init

BINSUBDIRS-y := src
BINSUBDIRS-$(CONFIG_BUILD_INIT) := src

INSTALL_DIR := bin

init_CFLAGS := $(USERLAND_CFLAGS)
init_LDFLAGS := $(USERLAND_LDFLAGS)

init_LIBS := libc

OBJ-y :=

include $(SRCDIR)/mk/bin.mk

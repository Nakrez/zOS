CURDIR := userland/bin/shell

BIN-y :=
BIN-$(CONFIG_SHELL) := shell

BINSUBDIRS-y :=
BINSUBDIRS-$(CONFIG_SHELL) := src

INSTALL_DIR := bin

shell_CFLAGS := $(USERLAND_CFLAGS)
shell_LDFLAGS := $(USERLAND_LDFLAGS)

shell_LIBS := libc

OBJ-y :=

include $(SRCDIR)/mk/bin.mk

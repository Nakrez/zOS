CURDIR := userland/bin/shell

BIN-y := shell
INSTALL_DIR := bin
BINSUBDIRS-y := src

shell_CFLAGS := $(USERLAND_CFLAGS)
shell_LDFLAGS := $(USERLAND_LDFLAGS)

shell_LIBS := libc

OBJ-y :=

include $(SRCDIR)/mk/bin.mk

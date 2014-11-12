CURDIR := userland/driver/tty

BIN-y :=
BIN-$(CONFIG_BUILD_TTY) := tty

BINSUBDIRS-y :=
BINSUBDIRS-$(CONFIG_BUILD_TTY) := src

INSTALL_DIR := bin

tty_CFLAGS := $(USERLAND_CFLAGS)
tty_LDFLAGS := $(USERLAND_LDFLAGS)

tty_LIBS := libdriver libc

OBJ-y :=

include $(SRCDIR)/mk/bin.mk

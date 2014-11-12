CURDIR := userland/driver/tty_ctrl

BIN-y :=
BIN-$(CONFIG_TTY) := tty_ctrl

BINSUBDIRS-y :=
BINSUBDIRS-$(CONFIG_TTY) := src

INSTALL_DIR := bin

tty_ctrl_CFLAGS := $(USERLAND_CFLAGS)
tty_ctrl_LDFLAGS := $(USERLAND_LDFLAGS)

tty_ctrl_LIBS := libdriver libc

OBJ-y :=

include $(SRCDIR)/mk/bin.mk

CURDIR := userland/driver/tty_ctrl

BIN-y := tty_ctrl
INSTALL_DIR := bin
BINSUBDIRS-y := src

tty_ctrl_CFLAGS := $(USERLAND_CFLAGS)
tty_ctrl_LDFLAGS := $(USERLAND_LDFLAGS)

tty_ctrl_LIBS := libdriver libc

OBJ-y :=

include $(SRCDIR)/mk/bin.mk

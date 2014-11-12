CURDIR := userland/driver/tty

BIN-y := tty
INSTALL_DIR := bin
BINSUBDIRS-y := src

tty_CFLAGS := $(USERLAND_CFLAGS)
tty_LDFLAGS := $(USERLAND_LDFLAGS)

tty_LIBS := libdriver libc

OBJ-y :=

include $(SRCDIR)/mk/bin.mk

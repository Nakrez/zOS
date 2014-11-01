CURDIR := userland/driver/tty_ctrl/

BIN := tty_ctrl
INSTALL_DIR := bin
BINSUBDIRS := src/

tty_ctrl_CFLAGS := $(USERLAND_CFLAGS)
tty_ctrl_LDFLAGS := $(USERLAND_LDFLAGS)

tty_ctrl_LIBS := libdriver libc

OBJ :=

include ${SRCDIR}mk/bin.mk

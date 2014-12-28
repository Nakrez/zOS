CURDIR := userland/bin/ls

BIN-y :=
BIN-$(CONFIG_BUILD_LS) := ls

BINSUBDIRS-y :=

INSTALL_DIR := bin

ls_CFLAGS := $(USERLAND_CFLAGS)
ls_LDFLAGS := $(USERLAND_LDFLAGS)

ls_LIBS := libc

OBJ-y :=
OBJ-$(CONFIG_BUILD_LS) := ls.o

include $(SRCDIR)/mk/bin.mk

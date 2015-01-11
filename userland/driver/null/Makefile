CURDIR := userland/driver/null

BIN-y :=
BIN-$(CONFIG_BUILD_NULL) := null

BINSUBDIRS-y :=

INSTALL_DIR := bin

null_CFLAGS := $(USERLAND_CFLAGS)
null_LDFLAGS := $(USERLAND_LDFLAGS)

null_LIBS := libdriver libc

OBJ-y := null.o

include $(SRCDIR)/mk/bin.mk

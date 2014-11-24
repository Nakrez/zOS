CURDIR := userland/bin/stat

BIN-y :=
BIN-$(CONFIG_BUILD_STAT) := stat

BINSUBDIRS-y :=

INSTALL_DIR := bin

stat_CFLAGS := $(USERLAND_CFLAGS)
stat_LDFLAGS := $(USERLAND_LDFLAGS)

stat_LIBS := libc

OBJ-y :=
OBJ-$(CONFIG_BUILD_STAT) := stat.o

include $(SRCDIR)/mk/bin.mk

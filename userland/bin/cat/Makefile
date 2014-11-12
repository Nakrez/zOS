CURDIR := userland/bin/cat

BIN-y :=
BIN-$(CONFIG_CAT) := cat

BINSUBDIRS-y :=

INSTALL_DIR := bin

cat_CFLAGS := $(USERLAND_CFLAGS)
cat_LDFLAGS := $(USERLAND_LDFLAGS)

cat_LIBS := libc

OBJ-y :=
OBJ-$(CONFIG_CAT) := cat.o

include $(SRCDIR)/mk/bin.mk

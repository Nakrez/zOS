CURDIR := userland/bin/cat

BIN-y := cat
INSTALL_DIR := bin
BINSUBDIRS-y :=

cat_CFLAGS := $(USERLAND_CFLAGS)
cat_LDFLAGS := $(USERLAND_LDFLAGS)

cat_LIBS := libc

OBJ-y := cat.o

include $(SRCDIR)/mk/bin.mk

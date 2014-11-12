CURDIR := userland/driver/kbd

BIN-y := kbd
INSTALL_DIR := bin
BINSUBDIRS-y := src

kbd_CFLAGS := $(USERLAND_CFLAGS)
kbd_CFLAGS += -I$(SRCDIR)/userland/driver/kbd/include/
kbd_CFLAGS += -I$(SRCDIR)/userland/driver/kbd/include/arch/${ZOS_ARCH}/

kbd_LDFLAGS := $(USERLAND_LDFLAGS)

kbd_LIBS := libdriver libc

OBJ-y :=

include $(SRCDIR)/mk/bin.mk

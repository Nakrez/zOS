CURDIR := userland/driver/kbd

BIN-y :=
BIN-$(CONFIG_BUILD_KBD) := kbd

BINSUBDIRS-y :=
BINSUBDIRS-$(CONFIG_BUILD_KBD) := src

INSTALL_DIR := bin

kbd_CFLAGS := $(USERLAND_CFLAGS)
kbd_CFLAGS += -I$(SRCDIR)/userland/driver/kbd/include/
kbd_CFLAGS += -I$(SRCDIR)/userland/driver/kbd/include/arch/${ZOS_ARCH}/

kbd_LDFLAGS := $(USERLAND_LDFLAGS)

kbd_LIBS := libdriver libc

OBJ-y :=

include $(SRCDIR)/mk/bin.mk

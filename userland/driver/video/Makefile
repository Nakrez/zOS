CURDIR := userland/driver/video

BIN-y :=
BIN-$(CONFIG_VIDEO) := video

BINSUBDIRS-y :=
BINSUBDIRS-$(CONFIG_VIDEO) := src

INSTALL_DIR := bin

video_CFLAGS := $(USERLAND_CFLAGS)
video_LDFLAGS := $(USERLAND_LDFLAGS)

video_LIBS := libdriver libc

OBJ-y :=

include $(SRCDIR)/mk/bin.mk

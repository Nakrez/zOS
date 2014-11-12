CURDIR := userland/driver/video

BIN-y := video
INSTALL_DIR := bin
BINSUBDIRS-y := src

video_CFLAGS := $(USERLAND_CFLAGS)
video_LDFLAGS := $(USERLAND_LDFLAGS)

video_LIBS := libdriver libc

OBJ-y :=

include $(SRCDIR)/mk/bin.mk

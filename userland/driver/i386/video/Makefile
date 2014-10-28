CURDIR := userland/driver/i386/video/

BIN := video
INSTALL_DIR := bin
BINSUBDIRS := src/

video_CFLAGS := -I${SRCDIR}userland/lib/libc/include
video_CFLAGS += -I${SRCDIR}userland/lib/libc/include/${ZOS_ARCH}/
video_CFLAGS += -I${SRCDIR}userland/lib/libdriver/include
video_CFLAGS += $(USERLAND_CFLAGS)

# video_LDFLAGS := $(USERLAND_LDFLAGS)
video_LDFLAGS := -T $(SRCDIR)/video.ld

video_LIBS := libdriver libc

OBJ :=

include ${SRCDIR}mk/bin.mk

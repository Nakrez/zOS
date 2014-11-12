CURDIR := userland/lib/libdriver

LIB-y := libdriver
LIBSUBDIRS-y := src

libdriver_CFLAGS := -I${SRCDIR}${CURDIR}/include
libdriver_CFLAGS += -I${SRCDIR}userland/lib/libc/include
libdriver_CFLAGS += -I${SRCDIR}userland/lib/libc/include/${ZOS_ARCH}/
libdriver_CFLAGS += $(USERLAND_CFLAGS)

OBJ-y :=

include $(SRCDIR)/mk/lib.mk

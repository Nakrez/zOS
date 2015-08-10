CURDIR := userland/lib/libfiu

LIB-y := libfiu
LIBSUBDIRS-y := src

libfiu_CFLAGS := -I${SRCDIR}${CURDIR}/include
libfiu_CFLAGS += -I${SRCDIR}userland/lib/libc/include
libfiu_CFLAGS += -I${SRCDIR}userland/lib/libc/include/${ZOS_ARCH}/
libfiu_CFLAGS += $(USERLAND_CFLAGS)

OBJ-y :=

include $(SRCDIR)/mk/lib.mk

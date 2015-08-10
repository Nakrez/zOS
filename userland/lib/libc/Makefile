CURDIR := userland/lib/libc

LIB-y := libc
LIBSUBDIRS-y := src

libc_CFLAGS := -I${SRCDIR}${CURDIR}/include
libc_CFLAGS += -I${SRCDIR}${CURDIR}/include/${ZOS_ARCH}/
libc_CFLAGS += $(USERLAND_CFLAGS)

OBJ-y :=

include $(SRCDIR)/mk/lib.mk

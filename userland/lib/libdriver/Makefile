CURDIR := userland/lib/libdriver/

LIB := libdriver
LIBSUBDIRS := src/

libdriver_CFLAGS := -I${SRCDIR}${CURDIR}/include
libdriver_CFLAGS += -I${SRCDIR}userland/lib/libc/include
libdriver_CFLAGS += -I${SRCDIR}userland/lib/libc/include/${ZOS_ARCH}/
libdriver_CFLAGS += -nostdinc -nostdlib

OBJ :=

include ${SRCDIR}mk/lib.mk

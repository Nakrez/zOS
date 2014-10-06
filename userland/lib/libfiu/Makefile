CURDIR := userland/lib/libfiu/

LIB := libfiu
LIBSUBDIRS := src/

libfiu_CFLAGS := -I${SRCDIR}${CURDIR}/include
libfiu_CFLAGS += -I${SRCDIR}userland/lib/libc/include
libfiu_CFLAGS += -I${SRCDIR}userland/lib/libc/include/${ZOS_ARCH}/
libfiu_CFLAGS += -nostdinc -nostdlib

OBJ :=

include ${SRCDIR}mk/lib.mk

CURDIR := userland/lib/libc/

LIB := libc
LIBSUBDIRS := src/

libc_CFLAGS := -I${SRCDIR}${CURDIR}/include
libc_CFLAGS += -I${SRCDIR}${CURDIR}/include/${ZOS_ARCH}/
libc_CFLAGS += -nostdinc -nostdlib -std=c99 -Wall -Wextra

OBJ :=

include ${SRCDIR}mk/lib.mk

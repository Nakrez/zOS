CURDIR := kernel/

BIN = zkernel
INSTALL_DIR := boot
BINSUBDIRS := core/ glue/$(ZOS_TARGET)/ arch/$(ZOS_ARCH)/

zkernel_CFLAGS := -I$(SRCDIR)include -I$(SRCDIR)include/kernel/arch/$(ZOS_ARCH)
zkernel_CFLAGS += -I$(SRCDIR)include/kernel/glue/$(ZOS_TARGET)/
zkernel_CFLAGS += -std=c99 -nostdinc -fno-builtin -fno-common -Wall -Wextra
zkernel_LDFLAGS := -T $(SRCDIR)$(CURDIR)arch/${ZOS_ARCH}/zkernel.ld

OBJ :=

include ${SRCDIR}mk/bin.mk

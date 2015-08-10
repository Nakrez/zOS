CURDIR := kernel

BIN-y = zkernel
BINSUBDIRS-y := core glue/$(ZOS_TARGET) arch/$(ZOS_ARCH)

INSTALL_DIR := boot

zkernel_CFLAGS := $(CFLAGS)
zkernel_CFLAGS += -I$(SRCDIR)/include -I$(SRCDIR)/include/kernel/arch/$(ZOS_ARCH)
zkernel_CFLAGS += -I$(SRCDIR)/include/kernel/glue/$(ZOS_TARGET)/
zkernel_CFLAGS += -std=c99 -nostdinc -fno-builtin -fno-common -Wall -Wextra
zkernel_LDFLAGS := -T $(SRCDIR)/$(CURDIR)/arch/$(ZOS_ARCH)/zkernel.ld

OBJ-y :=

include $(SRCDIR)/mk/bin.mk

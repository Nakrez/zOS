CURDIR := kernel

BIN-y = zkernel
INSTALL_DIR := boot
BINSUBDIRS-y := core glue/$(ZOS_TARGET) arch/$(ZOS_ARCH)

zkernel_CFLAGS := $(CFLAGS)
zkernel_CFLAGS += -I$(SRCDIR)/include -I$(SRCDIR)/include/kernel/arch/$(ZOS_ARCH)
zkernel_CFLAGS += -I$(SRCDIR)/include/kernel/glue/$(ZOS_TARGET)/
zkernel_CFLAGS += -std=c99 -nostdinc -fno-builtin -fno-common -Wall -Wextra
zkernel_LDFLAGS := -T $(SRCDIR)/$(CURDIR)/arch/$(ZOS_ARCH)/zkernel.ld

OBJ-y :=

include $(SRCDIR)/mk/bin.mk

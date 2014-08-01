CURDIR := bootloader/x86-pc/

BIN := zboot
INSTALL_DIR := boot
BINSUBDIRS :=

OBJ := entry.o

zboot_LDFLAGS = $(LDFLAGS) -T $(SRCDIR)$(CURDIR)zboot.ld

zos-x86-pc-image.img: rootfs.img

include ${SRCDIR}/mk/bin.mk

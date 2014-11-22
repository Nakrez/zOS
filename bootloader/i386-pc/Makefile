CURDIR := bootloader/i386-pc

BIN-y = zboot
INSTALL_DIR := boot
BINSUBDIRS-y :=

EXTRA_FILE := ${EXTRA_FILE} rootfs/boot/grub/grub.cfg

OBJ-y := entry.o bootloader.o utils.o

zboot_CFLAGS := $(CFLAGS)
zboot_CFLAGS += -I$(SRCDIR)/include -I$(SRCDIR)/include/kernel/arch/$(ZOS_ARCH)
zboot_CFLAGS += -std=c99 -nostdinc -fno-builtin -fno-common -Wall -Wextra
zboot_LDFLAGS := -T $(SRCDIR)/$(CURDIR)/zboot.ld

include $(SRCDIR)/mk/bin.mk

zos-i386-pc-image.img: $(INSTALL_BIN) ${EXTRA_FILE}
	$(call run,MKIMG,)
	@dd if=/dev/zero of=$@ bs=516096c count=40 > /dev/null 2>&1
	@echo -e "o\nn\np\n\n\n\na\nw\n" | fdisk $@ > /dev/null 2>&1
	@sudo mkdir -p /mnt/zos
	@sudo losetup /dev/loop0 $@
	@echo -e "y\n" | sudo mkfs.ext2 /dev/loop0 > /dev/null 2>&1
	@sudo mount -t ext2 /dev/loop0 /mnt/zos
	@sudo cp -r rootfs/* /mnt/zos
	@sudo grub-install --boot-directory=/mnt/zos/boot --force /dev/loop0 > /dev/null 2>&1
	@sudo umount /dev/loop0
	@sudo losetup -d /dev/loop0

rootfs/boot/grub/grub.cfg:
	$(call run,GRUB.CFG,)
	@echo "set timeout=0" > $@
	@echo "set default=0" >> $@
	@echo "menuentry "zOS" {" >> $@
	@echo "multiboot /boot/zboot" >> $@
	@echo "module /boot/zkernel" >> $@
	@echo "module /bin/init" >> $@
ifeq ($(CONFIG_BUILD_ATA),y)
	@echo "module /bin/ata" >> $@
endif
ifeq ($(CONFIG_BUILD_AHCI),y)
	@echo "module /bin/ahci" >> $@
endif
	@echo "module /bin/ext2" >> $@
	@echo "boot" >> $@
	@echo "}" >> $@

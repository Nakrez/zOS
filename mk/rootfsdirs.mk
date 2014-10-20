ROOTFSDIRS := rootfs
ROOTFSDIRS += rootfs/boot rootfs/boot/grub rootfs/boot/grub/i386-pc
ROOTFSDIRS += rootfs/bin rootfs/etc rootfs/dev

$(foreach RD, $(ROOTFSDIRS), $(shell mkdir -p $(RD)))

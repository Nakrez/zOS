ROOTFSDIRS := rootfs
ROOTFSDIRS += rootfs/boot
ROOTFSDIRS += rootfs/bin

$(foreach RD, $(ROOTFSDIRS), $(shell mkdir -p $(RD)))

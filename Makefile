# Some parts of this Makefile comes from Linux
ARCH ?= i386
PLAT ?= pc

ZOS_ARCH := $(ARCH)
ZOS_PLAT := $(PLAT)

ZOS_TARGET = ${ZOS_ARCH}-${ZOS_PLAT}

export ZOS_ARCH ZOS_PLAT ZOS_TARGET

# Do not use make's built-in rules and variables
# (this increases performance and avoids hard-to-debug behaviour);
MAKEFLAGS += -rR

# Avoid funny character set dependencies
unexport LC_ALL
LC_COLLATE=C
LC_NUMERIC=C
export LC_COLLATE LC_NUMERIC

# Avoid interference with shell env settings
unexport GREP_OPTIONS

# Kconfig BUILD

# Do not:
# o  use make's built-in rules and variables
#    (this increases performance and avoids hard-to-debug behaviour);
# o  print "Entering directory ...";
MAKEFLAGS += -rR --no-print-directory

# To put more focus on warnings, be less verbose as default
# Use 'make V=1' to see the full commands

ifdef V
  ifeq ("$(origin V)", "command line")
    KBUILD_VERBOSE = $(V)
  endif
endif
ifndef KBUILD_VERBOSE
  KBUILD_VERBOSE = 0
endif

export KBUILD_VERBOSE

PHONY = all

all: zos

KCURDIR := $(shell pwd)

# Cancel implicit rules on top Makefile
$(KCURDIR)/Makefile Makefile: ;

KCONFIG_AUTOHEADER = $(srctree)/include/kernel/config.h
KBUILD_KCONFIG = $(KCURDIR)/Kconfig
export KBUILD_KCONFIG KCONFIG_AUTOHEADER

# SHELL used by kbuild
CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
	  else if [ -x /bin/bash ]; then echo /bin/bash; \
	  else echo sh; fi ; fi)

# Make variables (CC, etc...)
HOSTCC       = gcc
HOSTCXX      = g++
HOSTCFLAGS   = -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer
HOSTCXXFLAGS = -O2

export HOSTCC HOSTCXX HOSTCFLAGS HOSTCXXFLAGS

srctree		:= $(KCURDIR)
objtree		:= $(KCURDIR)
src		:= $(srctree)
obj		:= $(objtree)

export srctree objtree

KCONFIG_CONFIG	?= .config
export KCONFIG_CONFIG

# We need some generic definitions (do not try to remake the file).
$(srctree)/scripts/Kbuild.include: ;
include $(srctree)/scripts/Kbuild.include

# Basic helpers built in scripts/
PHONY += scripts_basic
scripts_basic:
	$(Q)$(MAKE) $(build)=scripts/basic


config: scripts_basic FORCE
	$(Q)$(MAKE) $(build)=scripts/kconfig $@

%config: scripts_basic FORCE
	$(Q)$(MAKE) $(build)=scripts/kconfig $@

PHONY += FORCE
FORCE:

# Declare the contents of the .PHONY variable as phony.  We keep that
# information in a variable so we can use it in if_changed and friends.
.PHONY: $(PHONY)

# ZOS BUILD

CC ?= gcc
LD ?= gcc
AS := gcc
AR ?= ar

CC := $(CROSS_COMPILE)$(CC)
LD := $(CROSS_COMPILE)$(LD)
AS := $(CROSS_COMPILE)$(AS)
AR := $(CROSS_COMPILE)$(AR)

CURDIR :=

SRCDIR := $(shell pwd)

DEPS :=

DESTDIR := $(shell pwd)

SUBDIRS := kernel userland bootloader/${ZOS_TARGET}

USERLAND_CFLAGS := $(CFLAGS)
USERLAND_CFLAGS += -I $(SRCDIR)/userland/lib/include/
USERLAND_CFLAGS += -I $(SRCDIR)/userland/lib/include/${ZOS_ARCH}/
USERLAND_CFLAGS += -std=c99 -Wall -Wextra -nostdinc -fno-builtin -fno-common

DEFAULT_LDFLAGS := $(DEFAULT_LDFLAGS) -nostdlib
DEFAULT_ASFLAGS := $(ASFLAGS)

USERLAND_LDFLAGS := -T $(SRCDIR)/userland/lib/libc/src/arch/$(ZOS_ARCH)/bin.ld

INSTALL_BIN :=
ALL_BIN :=
EXTRA_FILE := rootfs/etc/init_conf

-include $(KCONFIG_CONFIG)

zos: silentoldconfig zos-$(ZOS_TARGET)-image.img

boot: zos
	$(call run,QEMU,)
	@qemu-system-i386 -vga std zos-$(ZOS_TARGET)-image.img -serial stdio

boot-gdb: zos
	$(call run,QEMU-GDB,)
	@qemu-system-i386 -vga std zos-$(ZOS_TARGET)-image.img -S -s -serial stdio

rootfs/%: $(SRCDIR)/userland/root/%
	$(call run,CP,cp $^ $@)

include $(SRCDIR)/mk/subdirs.mk

clean:
	@find . $(RCS_FIND_IGNORE) \
		\( -name '*.[oas]' -o -name '*.ko' -o -name '.*.cmd' \
		-o -name '.*.d' -o -name '.*.tmp' -o -name '*.mod.c' \
		-o -name 'Module.markers' -o -name '.tmp_*.o.*' \) \
		-type f -print | xargs rm -f -v

distclean: clean
	@rm -rf $(ALL_BIN)
	@rm -rf $(ALL_LIB)
	@rm -rf $(DEPS)
	@rm -rf rootfs.img rootfs zos-$(ZOS_TARGET)-image.img

include $(SRCDIR)/mk/rootfsdirs.mk
include $(SRCDIR)/mk/run.mk
include $(SRCDIR)/mk/rules.mk

# zOS #

zOS is a hobby operating system, built form scratch, in development since
November of 2013. It has been totally rewritten from scratch since
August 2014.

zOS is based on a micro kernel (named zKernel), that has the particularity of
having the Virtual File System embedded within the kernel.

## Features ##

### Kernel features ###

* Memory management
* Process management:
  * ELF loader (static ELF only)
  * Round robin scheduler
  * Thread support
  * fork() using Copy-On-Write
* Interrupt in userland API
* VFS
  * Driver interface and communication
  * Filesystem Driver interface (WIP)

### Userland features ###

* C library
  * Provide syscall interface
* Driver library (libdriver)
  * Easy implementation of drivers
  * Only provide device information and operation, the library do everything
    else
* Filesystem In Userland library (libfiu)
  * Easy filesystem implementation
  * Filesystem drivers only provide filesystem operations and informations, the
    library do the rest
  * Generic block cache implementation
* Drivers:
  * ATA: (only disk no CDROM atm)
  * Ext2
  * Keyboard
  * Video (x86 only using VBE Extended)
  * TTY
* Other binaries:
  * Shell, cat, ls, stat

## Build instruction ##

zOS use Kconfig build system.

To create a new configuration file you can type `make menuconfig` or
`make config`.

Some system already have a default configuration file. To use it just type
`make $TARGET_defconfig`, where $TARGET is the target you want to build (for
example to build zos for i386-pc the target is `i386_pc`). All configuration
files are available in `kernel/arch/$ARCH/configs` (where $ARCH is the
architecture you want to build zos for).

Then, you just have to run `make` to create the image. If you want to create
the image and run it in qemu you can use `make boot`.

If you are building zOS for a different architecture than your host computer
be sure to set variables `CROSS_COMPILE`, `ARCH` and `PLAT` correctly.

If you want to see all available make targets just type `make help`.

## License ##

See COPYING file

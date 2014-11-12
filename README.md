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
  * Shell, cat

## Build instruction ##

zOS use Kconfig build system.

To create a new configuration file you can type `make menuconfig` or
`make config`.

Then, you just have to run `make` or `make boot` if you want to run the
created image file into `qemu`.

If you are building zOS for a different architecture than your host computer
be sure to correctly set `CROSS_COMPILE`.

## License ##

See COPYING file

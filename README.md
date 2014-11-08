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

To build zOS you first have to configure it by lanching the `configure` script.

If you want to build the image just use `make`.

You can boot zOS by typing `make boot`. It will generate the image and launch
qemu with it.

## License ##

See COPYING file

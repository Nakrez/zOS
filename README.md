# zOS #

zOS is a hobby operating system, built form scratch, in development since
November of 2013. It has been totally rewritten from scratch since
August 2014.

zOS is based on a micro kernel (named zKernel), that has the particularity of
having the Virtual File System embedded with the kernel.

## Features ##

### Kernel features ###

* Memory management
* Process management:
  * ELF loader (static ELF only)
  * Round robin scheduler
  * Thread support
  * fork()
* VFS
  * Driver interface and communication
  * Filesystem Driver interface (WIP)

### Userland features ###

* C library
  * Provide syscall interface
* Drivers are a work in progress

## Build instruction ##

To build zOS you first have to configure it by lanching the configure script.

If you want to build the image just use `make`.

You can boot zOS by typing `make boot`. It will generate the image and launch
qemu with it.

## License ##

See COPYING file

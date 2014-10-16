#ifndef ERRNO_H
# define ERRNO_H

# define EPERM 1 /* Operation not permitted */
# define ENOENT 2 /* No such file or directory */
# define EBADF 9 /* Bad file number */
# define ENOMEM 12 /* Out of memory */
# define EACCES 13 /* Permission denied */
# define EFAULT 14 /* Bad address */
# define EBUSY 16 /* Device or resource busy */
# define EEXIST 17 /* File exists */
# define ENODEV 19 /* No such device */
# define ENOTDIR 20 /* Not a directory */
# define EISDIR 21 /* Is a directory */
# define EINVAL 22 /* Invalid argument */
# define EMFILE 24 /* Too many open files */
# define ENAMETOOLONG 36 /* File name too long */
# define ENOSYS 38 /* Function not implemented */
# define EBADE 52 /* Invalid exchange */
# define ENODATA 61 /* No data available */
# define ELIBBAD 80 /* Accessing a corrupted shared library */

#endif /* !ERRNO_H */

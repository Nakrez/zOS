#ifndef LIBC_STDIO_H
# define LIBC_STDIO_H

# include <stdarg.h>

# define _IOFBF 1
# define _IOLBF 2
# define _IONBF 3

struct _IO_FILE;

typedef struct _IO_FILE FILE;

FILE *fopen(const char *filename, const char *mode);

int sprintf(char *str, const char *format, ...);
int vsprintf(char *str, const char *format, va_list ap);

#endif /* !LIBC_STDIO_H */

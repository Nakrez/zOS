#ifndef LIBC_STDIO_H
# define LIBC_STDIO_H

# include <stdarg.h>

int sprintf(char *str, const char *format, ...);
int vsprintf(char *str, const char *format, va_list ap);

#endif /* !LIBC_STDIO_H */

#ifndef ZOS_H
# define ZOS_H

# ifndef NULL
#  define NULL ((void *)0)
# endif /* !NULL */

# define KB (1024)
# define MB (KB * 1024)

typedef __builtin_va_list va_list;

# define __user

# define va_start(v, l) __builtin_va_start(v, l)
# define va_end(v) __builtin_va_end(v)
# define va_arg(v, l) __builtin_va_arg(v, l)

# define align(data, a) ((data + a - 1) & ~(a - 1))

# define glue_call(subsystem, method, args...)                  \
    ({                                                          \
        int ret = 0;                                            \
        if (subsystem ## _glue_dispatcher.method)               \
            ret = subsystem ## _glue_dispatcher.method(args);   \
        ret;                                                    \
    })

# define __unused __attribute__((unused))

#endif /* !ZOS_H */

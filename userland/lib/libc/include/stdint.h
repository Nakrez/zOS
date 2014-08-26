#ifndef LIBC_STDINT_H
# define LIBC_STDINT_H

# include <sys/types.h>

typedef __i8 int8_t;
typedef __u8 uint8_t;
typedef __i16 int16_t;
typedef __u16 uint16_t;
typedef __i32 int32_t;
typedef __u32 uint32_t;
typedef __i64 int64_t;
typedef __u64 uint64_t;

typedef __size size_t;

#endif /* !LIBC_STDINT_H */

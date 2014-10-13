#ifndef I386_SYS_TYPES_H
# define I386_SYS_TYPES_H

typedef signed char __i8;
typedef unsigned char __u8;
typedef signed short __i16;
typedef unsigned short __u16;
typedef signed int __i32;
typedef unsigned int __u32;
typedef signed long long __i64;
typedef unsigned long long __u64;

typedef __u32 __uptr;

typedef __u32 __size;

typedef __i32 pid_t;

typedef __u32 ino_t;
typedef __u32 mode_t;
typedef __i32 dev_t;
typedef __u32 nlink_t;
typedef __u32 time_t;

typedef __u16 uid_t;
typedef __u16 gid_t;

typedef __i32 blksize_t;
typedef __u64 blkcnt_t;

typedef __i32 off_t;

typedef __u32 vop_t;

#endif /* !I386_SYS_TYPES_H */

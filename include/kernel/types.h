/*
Copyright (C) 2013, 2014 Baptiste Covolato <b.covolato@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef TYPES_H
# define TYPES_H

# include <arch/types.h>

# define UINT_MAX (~0U)

# define TICK_MAX (UINT_MAX - 1)

typedef __i8 int8_t;
typedef __u8 uint8_t;

typedef __i16 int16_t;
typedef __u16 uint16_t;

typedef __i32 int32_t;
typedef __u32 uint32_t;

typedef __i64 int64_t;
typedef __u64 uint64_t;

typedef __u32 size_t;

typedef __uptr uintptr_t;
typedef __uptr paddr_t;
typedef __uptr vaddr_t;

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

typedef __i64 off_t;

typedef __u32 vop_t;

typedef __u32 tick_t;

#endif /* !TYPES_H */

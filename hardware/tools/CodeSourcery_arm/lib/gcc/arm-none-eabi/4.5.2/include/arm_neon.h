/* ARM NEON intrinsics include file. This file is generated automatically
   using neon-gen.ml.  Please do not edit manually.

   Copyright (C) 2006, 2007, 2009 Free Software Foundation, Inc.
   Contributed by CodeSourcery.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _GCC_ARM_NEON_H
#define _GCC_ARM_NEON_H 1

#ifndef __ARM_NEON__
#error You must enable NEON instructions (e.g. -mfloat-abi=softfp -mfpu=neon) to use arm_neon.h
#else

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef __builtin_neon_qi int8x8_t	__attribute__ ((__vector_size__ (8)));
typedef __builtin_neon_hi int16x4_t	__attribute__ ((__vector_size__ (8)));
typedef __builtin_neon_si int32x2_t	__attribute__ ((__vector_size__ (8)));
typedef __builtin_neon_di int64x1_t;
typedef __builtin_neon_sf float32x2_t	__attribute__ ((__vector_size__ (8)));
typedef __builtin_neon_poly8 poly8x8_t	__attribute__ ((__vector_size__ (8)));
typedef __builtin_neon_poly16 poly16x4_t	__attribute__ ((__vector_size__ (8)));
typedef __builtin_neon_uqi uint8x8_t	__attribute__ ((__vector_size__ (8)));
typedef __builtin_neon_uhi uint16x4_t	__attribute__ ((__vector_size__ (8)));
typedef __builtin_neon_usi uint32x2_t	__attribute__ ((__vector_size__ (8)));
typedef __builtin_neon_udi uint64x1_t;
typedef __builtin_neon_qi int8x16_t	__attribute__ ((__vector_size__ (16)));
typedef __builtin_neon_hi int16x8_t	__attribute__ ((__vector_size__ (16)));
typedef __builtin_neon_si int32x4_t	__attribute__ ((__vector_size__ (16)));
typedef __builtin_neon_di int64x2_t	__attribute__ ((__vector_size__ (16)));
typedef __builtin_neon_sf float32x4_t	__attribute__ ((__vector_size__ (16)));
typedef __builtin_neon_poly8 poly8x16_t	__attribute__ ((__vector_size__ (16)));
typedef __builtin_neon_poly16 poly16x8_t	__attribute__ ((__vector_size__ (16)));
typedef __builtin_neon_uqi uint8x16_t	__attribute__ ((__vector_size__ (16)));
typedef __builtin_neon_uhi uint16x8_t	__attribute__ ((__vector_size__ (16)));
typedef __builtin_neon_usi uint32x4_t	__attribute__ ((__vector_size__ (16)));
typedef __builtin_neon_udi uint64x2_t	__attribute__ ((__vector_size__ (16)));

typedef float float32_t;
typedef __builtin_neon_poly8 poly8_t;
typedef __builtin_neon_poly16 poly16_t;

typedef struct int8x8x2_t
{
  int8x8_t val[2];
} int8x8x2_t;

typedef struct int8x16x2_t
{
  int8x16_t val[2];
} int8x16x2_t;

typedef struct int16x4x2_t
{
  int16x4_t val[2];
} int16x4x2_t;

typedef struct int16x8x2_t
{
  int16x8_t val[2];
} int16x8x2_t;

typedef struct int32x2x2_t
{
  int32x2_t val[2];
} int32x2x2_t;

typedef struct int32x4x2_t
{
  int32x4_t val[2];
} int32x4x2_t;

typedef struct int64x1x2_t
{
  int64x1_t val[2];
} int64x1x2_t;

typedef struct int64x2x2_t
{
  int64x2_t val[2];
} int64x2x2_t;

typedef struct uint8x8x2_t
{
  uint8x8_t val[2];
} uint8x8x2_t;

typedef struct uint8x16x2_t
{
  uint8x16_t val[2];
} uint8x16x2_t;

typedef struct uint16x4x2_t
{
  uint16x4_t val[2];
} uint16x4x2_t;

typedef struct uint16x8x2_t
{
  uint16x8_t val[2];
} uint16x8x2_t;

typedef struct uint32x2x2_t
{
  uint32x2_t val[2];
} uint32x2x2_t;

typedef struct uint32x4x2_t
{
  uint32x4_t val[2];
} uint32x4x2_t;

typedef struct uint64x1x2_t
{
  uint64x1_t val[2];
} uint64x1x2_t;

typedef struct uint64x2x2_t
{
  uint64x2_t val[2];
} uint64x2x2_t;

typedef struct float32x2x2_t
{
  float32x2_t val[2];
} float32x2x2_t;

typedef struct float32x4x2_t
{
  float32x4_t val[2];
} float32x4x2_t;

typedef struct poly8x8x2_t
{
  poly8x8_t val[2];
} poly8x8x2_t;

typedef struct poly8x16x2_t
{
  poly8x16_t val[2];
} poly8x16x2_t;

typedef struct poly16x4x2_t
{
  poly16x4_t val[2];
} poly16x4x2_t;

typedef struct poly16x8x2_t
{
  poly16x8_t val[2];
} poly16x8x2_t;

typedef struct int8x8x3_t
{
  int8x8_t val[3];
} int8x8x3_t;

typedef struct int8x16x3_t
{
  int8x16_t val[3];
} int8x16x3_t;

typedef struct int16x4x3_t
{
  int16x4_t val[3];
} int16x4x3_t;

typedef struct int16x8x3_t
{
  int16x8_t val[3];
} int16x8x3_t;

typedef struct int32x2x3_t
{
  int32x2_t val[3];
} int32x2x3_t;

typedef struct int32x4x3_t
{
  int32x4_t val[3];
} int32x4x3_t;

typedef struct int64x1x3_t
{
  int64x1_t val[3];
} int64x1x3_t;

typedef struct int64x2x3_t
{
  int64x2_t val[3];
} int64x2x3_t;

typedef struct uint8x8x3_t
{
  uint8x8_t val[3];
} uint8x8x3_t;

typedef struct uint8x16x3_t
{
  uint8x16_t val[3];
} uint8x16x3_t;

typedef struct uint16x4x3_t
{
  uint16x4_t val[3];
} uint16x4x3_t;

typedef struct uint16x8x3_t
{
  uint16x8_t val[3];
} uint16x8x3_t;

typedef struct uint32x2x3_t
{
  uint32x2_t val[3];
} uint32x2x3_t;

typedef struct uint32x4x3_t
{
  uint32x4_t val[3];
} uint32x4x3_t;

typedef struct uint64x1x3_t
{
  uint64x1_t val[3];
} uint64x1x3_t;

typedef struct uint64x2x3_t
{
  uint64x2_t val[3];
} uint64x2x3_t;

typedef struct float32x2x3_t
{
  float32x2_t val[3];
} float32x2x3_t;

typedef struct float32x4x3_t
{
  float32x4_t val[3];
} float32x4x3_t;

typedef struct poly8x8x3_t
{
  poly8x8_t val[3];
} poly8x8x3_t;

typedef struct poly8x16x3_t
{
  poly8x16_t val[3];
} poly8x16x3_t;

typedef struct poly16x4x3_t
{
  poly16x4_t val[3];
} poly16x4x3_t;

typedef struct poly16x8x3_t
{
  poly16x8_t val[3];
} poly16x8x3_t;

typedef struct int8x8x4_t
{
  int8x8_t val[4];
} int8x8x4_t;

typedef struct int8x16x4_t
{
  int8x16_t val[4];
} int8x16x4_t;

typedef struct int16x4x4_t
{
  int16x4_t val[4];
} int16x4x4_t;

typedef struct int16x8x4_t
{
  int16x8_t val[4];
} int16x8x4_t;

typedef struct int32x2x4_t
{
  int32x2_t val[4];
} int32x2x4_t;

typedef struct int32x4x4_t
{
  int32x4_t val[4];
} int32x4x4_t;

typedef struct int64x1x4_t
{
  int64x1_t val[4];
} int64x1x4_t;

typedef struct int64x2x4_t
{
  int64x2_t val[4];
} int64x2x4_t;

typedef struct uint8x8x4_t
{
  uint8x8_t val[4];
} uint8x8x4_t;

typedef struct uint8x16x4_t
{
  uint8x16_t val[4];
} uint8x16x4_t;

typedef struct uint16x4x4_t
{
  uint16x4_t val[4];
} uint16x4x4_t;

typedef struct uint16x8x4_t
{
  uint16x8_t val[4];
} uint16x8x4_t;

typedef struct uint32x2x4_t
{
  uint32x2_t val[4];
} uint32x2x4_t;

typedef struct uint32x4x4_t
{
  uint32x4_t val[4];
} uint32x4x4_t;

typedef struct uint64x1x4_t
{
  uint64x1_t val[4];
} uint64x1x4_t;

typedef struct uint64x2x4_t
{
  uint64x2_t val[4];
} uint64x2x4_t;

typedef struct float32x2x4_t
{
  float32x2_t val[4];
} float32x2x4_t;

typedef struct float32x4x4_t
{
  float32x4_t val[4];
} float32x4x4_t;

typedef struct poly8x8x4_t
{
  poly8x8_t val[4];
} poly8x8x4_t;

typedef struct poly8x16x4_t
{
  poly8x16_t val[4];
} poly8x16x4_t;

typedef struct poly16x4x4_t
{
  poly16x4_t val[4];
} poly16x4x4_t;

typedef struct poly16x8x4_t
{
  poly16x8_t val[4];
} poly16x8x4_t;


__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vadd_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vaddv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vadd_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vaddv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vadd_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vaddv2si (__a, __b, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vadd_f32 (float32x2_t __a, float32x2_t __b)
{
  return (float32x2_t)__builtin_neon_vaddv2sf (__a, __b, 3);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vadd_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vaddv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vadd_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vaddv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vadd_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vaddv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vadd_s64 (int64x1_t __a, int64x1_t __b)
{
  return (int64x1_t)__builtin_neon_vadddi (__a, __b, 1);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vadd_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return (uint64x1_t)__builtin_neon_vadddi ((int64x1_t) __a, (int64x1_t) __b, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vaddq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t)__builtin_neon_vaddv16qi (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vaddq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vaddv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vaddq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vaddv4si (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vaddq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (int64x2_t)__builtin_neon_vaddv2di (__a, __b, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vaddq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (float32x4_t)__builtin_neon_vaddv4sf (__a, __b, 3);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vaddq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vaddv16qi ((int8x16_t) __a, (int8x16_t) __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vaddq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vaddv8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vaddq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vaddv4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vaddq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return (uint64x2_t)__builtin_neon_vaddv2di ((int64x2_t) __a, (int64x2_t) __b, 0);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vaddl_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int16x8_t)__builtin_neon_vaddlv8qi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vaddl_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int32x4_t)__builtin_neon_vaddlv4hi (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vaddl_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int64x2_t)__builtin_neon_vaddlv2si (__a, __b, 1);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vaddl_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vaddlv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vaddl_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vaddlv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vaddl_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint64x2_t)__builtin_neon_vaddlv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vaddw_s8 (int16x8_t __a, int8x8_t __b)
{
  return (int16x8_t)__builtin_neon_vaddwv8qi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vaddw_s16 (int32x4_t __a, int16x4_t __b)
{
  return (int32x4_t)__builtin_neon_vaddwv4hi (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vaddw_s32 (int64x2_t __a, int32x2_t __b)
{
  return (int64x2_t)__builtin_neon_vaddwv2si (__a, __b, 1);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vaddw_u8 (uint16x8_t __a, uint8x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vaddwv8qi ((int16x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vaddw_u16 (uint32x4_t __a, uint16x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vaddwv4hi ((int32x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vaddw_u32 (uint64x2_t __a, uint32x2_t __b)
{
  return (uint64x2_t)__builtin_neon_vaddwv2si ((int64x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vhadd_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vhaddv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vhadd_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vhaddv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vhadd_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vhaddv2si (__a, __b, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vhadd_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vhaddv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vhadd_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vhaddv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vhadd_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vhaddv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vhaddq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t)__builtin_neon_vhaddv16qi (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vhaddq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vhaddv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vhaddq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vhaddv4si (__a, __b, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vhaddq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vhaddv16qi ((int8x16_t) __a, (int8x16_t) __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vhaddq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vhaddv8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vhaddq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vhaddv4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vrhadd_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vhaddv8qi (__a, __b, 5);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vrhadd_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vhaddv4hi (__a, __b, 5);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vrhadd_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vhaddv2si (__a, __b, 5);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vrhadd_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vhaddv8qi ((int8x8_t) __a, (int8x8_t) __b, 4);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vrhadd_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vhaddv4hi ((int16x4_t) __a, (int16x4_t) __b, 4);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vrhadd_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vhaddv2si ((int32x2_t) __a, (int32x2_t) __b, 4);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vrhaddq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t)__builtin_neon_vhaddv16qi (__a, __b, 5);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vrhaddq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vhaddv8hi (__a, __b, 5);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vrhaddq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vhaddv4si (__a, __b, 5);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vrhaddq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vhaddv16qi ((int8x16_t) __a, (int8x16_t) __b, 4);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vrhaddq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vhaddv8hi ((int16x8_t) __a, (int16x8_t) __b, 4);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vrhaddq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vhaddv4si ((int32x4_t) __a, (int32x4_t) __b, 4);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vqadd_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vqaddv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vqadd_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vqaddv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vqadd_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vqaddv2si (__a, __b, 1);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vqadd_s64 (int64x1_t __a, int64x1_t __b)
{
  return (int64x1_t)__builtin_neon_vqadddi (__a, __b, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vqadd_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vqaddv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vqadd_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vqaddv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vqadd_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vqaddv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vqadd_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return (uint64x1_t)__builtin_neon_vqadddi ((int64x1_t) __a, (int64x1_t) __b, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vqaddq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t)__builtin_neon_vqaddv16qi (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vqaddq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vqaddv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqaddq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vqaddv4si (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vqaddq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (int64x2_t)__builtin_neon_vqaddv2di (__a, __b, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vqaddq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vqaddv16qi ((int8x16_t) __a, (int8x16_t) __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vqaddq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vqaddv8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vqaddq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vqaddv4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vqaddq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return (uint64x2_t)__builtin_neon_vqaddv2di ((int64x2_t) __a, (int64x2_t) __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vaddhn_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int8x8_t)__builtin_neon_vaddhnv8hi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vaddhn_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int16x4_t)__builtin_neon_vaddhnv4si (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vaddhn_s64 (int64x2_t __a, int64x2_t __b)
{
  return (int32x2_t)__builtin_neon_vaddhnv2di (__a, __b, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vaddhn_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vaddhnv8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vaddhn_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vaddhnv4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vaddhn_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vaddhnv2di ((int64x2_t) __a, (int64x2_t) __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vraddhn_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int8x8_t)__builtin_neon_vaddhnv8hi (__a, __b, 5);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vraddhn_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int16x4_t)__builtin_neon_vaddhnv4si (__a, __b, 5);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vraddhn_s64 (int64x2_t __a, int64x2_t __b)
{
  return (int32x2_t)__builtin_neon_vaddhnv2di (__a, __b, 5);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vraddhn_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vaddhnv8hi ((int16x8_t) __a, (int16x8_t) __b, 4);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vraddhn_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vaddhnv4si ((int32x4_t) __a, (int32x4_t) __b, 4);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vraddhn_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vaddhnv2di ((int64x2_t) __a, (int64x2_t) __b, 4);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vmul_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vmulv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vmul_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vmulv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vmul_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vmulv2si (__a, __b, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vmul_f32 (float32x2_t __a, float32x2_t __b)
{
  return (float32x2_t)__builtin_neon_vmulv2sf (__a, __b, 3);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vmul_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vmulv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vmul_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vmulv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vmul_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vmulv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vmul_p8 (poly8x8_t __a, poly8x8_t __b)
{
  return (poly8x8_t)__builtin_neon_vmulv8qi ((int8x8_t) __a, (int8x8_t) __b, 2);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vmulq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t)__builtin_neon_vmulv16qi (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vmulq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vmulv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmulq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vmulv4si (__a, __b, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vmulq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (float32x4_t)__builtin_neon_vmulv4sf (__a, __b, 3);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vmulq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vmulv16qi ((int8x16_t) __a, (int8x16_t) __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vmulq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vmulv8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmulq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vmulv4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vmulq_p8 (poly8x16_t __a, poly8x16_t __b)
{
  return (poly8x16_t)__builtin_neon_vmulv16qi ((int8x16_t) __a, (int8x16_t) __b, 2);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vqdmulh_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vqdmulhv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vqdmulh_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vqdmulhv2si (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vqdmulhq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vqdmulhv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqdmulhq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vqdmulhv4si (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vqrdmulh_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vqdmulhv4hi (__a, __b, 5);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vqrdmulh_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vqdmulhv2si (__a, __b, 5);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vqrdmulhq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vqdmulhv8hi (__a, __b, 5);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqrdmulhq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vqdmulhv4si (__a, __b, 5);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vmull_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int16x8_t)__builtin_neon_vmullv8qi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmull_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int32x4_t)__builtin_neon_vmullv4hi (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vmull_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int64x2_t)__builtin_neon_vmullv2si (__a, __b, 1);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vmull_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vmullv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmull_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vmullv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vmull_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint64x2_t)__builtin_neon_vmullv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vmull_p8 (poly8x8_t __a, poly8x8_t __b)
{
  return (poly16x8_t)__builtin_neon_vmullv8qi ((int8x8_t) __a, (int8x8_t) __b, 2);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqdmull_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int32x4_t)__builtin_neon_vqdmullv4hi (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vqdmull_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int64x2_t)__builtin_neon_vqdmullv2si (__a, __b, 1);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vmla_s8 (int8x8_t __a, int8x8_t __b, int8x8_t __c)
{
  return (int8x8_t)__builtin_neon_vmlav8qi (__a, __b, __c, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vmla_s16 (int16x4_t __a, int16x4_t __b, int16x4_t __c)
{
  return (int16x4_t)__builtin_neon_vmlav4hi (__a, __b, __c, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vmla_s32 (int32x2_t __a, int32x2_t __b, int32x2_t __c)
{
  return (int32x2_t)__builtin_neon_vmlav2si (__a, __b, __c, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vmla_f32 (float32x2_t __a, float32x2_t __b, float32x2_t __c)
{
  return (float32x2_t)__builtin_neon_vmlav2sf (__a, __b, __c, 3);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vmla_u8 (uint8x8_t __a, uint8x8_t __b, uint8x8_t __c)
{
  return (uint8x8_t)__builtin_neon_vmlav8qi ((int8x8_t) __a, (int8x8_t) __b, (int8x8_t) __c, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vmla_u16 (uint16x4_t __a, uint16x4_t __b, uint16x4_t __c)
{
  return (uint16x4_t)__builtin_neon_vmlav4hi ((int16x4_t) __a, (int16x4_t) __b, (int16x4_t) __c, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vmla_u32 (uint32x2_t __a, uint32x2_t __b, uint32x2_t __c)
{
  return (uint32x2_t)__builtin_neon_vmlav2si ((int32x2_t) __a, (int32x2_t) __b, (int32x2_t) __c, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vmlaq_s8 (int8x16_t __a, int8x16_t __b, int8x16_t __c)
{
  return (int8x16_t)__builtin_neon_vmlav16qi (__a, __b, __c, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vmlaq_s16 (int16x8_t __a, int16x8_t __b, int16x8_t __c)
{
  return (int16x8_t)__builtin_neon_vmlav8hi (__a, __b, __c, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmlaq_s32 (int32x4_t __a, int32x4_t __b, int32x4_t __c)
{
  return (int32x4_t)__builtin_neon_vmlav4si (__a, __b, __c, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vmlaq_f32 (float32x4_t __a, float32x4_t __b, float32x4_t __c)
{
  return (float32x4_t)__builtin_neon_vmlav4sf (__a, __b, __c, 3);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vmlaq_u8 (uint8x16_t __a, uint8x16_t __b, uint8x16_t __c)
{
  return (uint8x16_t)__builtin_neon_vmlav16qi ((int8x16_t) __a, (int8x16_t) __b, (int8x16_t) __c, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vmlaq_u16 (uint16x8_t __a, uint16x8_t __b, uint16x8_t __c)
{
  return (uint16x8_t)__builtin_neon_vmlav8hi ((int16x8_t) __a, (int16x8_t) __b, (int16x8_t) __c, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmlaq_u32 (uint32x4_t __a, uint32x4_t __b, uint32x4_t __c)
{
  return (uint32x4_t)__builtin_neon_vmlav4si ((int32x4_t) __a, (int32x4_t) __b, (int32x4_t) __c, 0);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vmlal_s8 (int16x8_t __a, int8x8_t __b, int8x8_t __c)
{
  return (int16x8_t)__builtin_neon_vmlalv8qi (__a, __b, __c, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmlal_s16 (int32x4_t __a, int16x4_t __b, int16x4_t __c)
{
  return (int32x4_t)__builtin_neon_vmlalv4hi (__a, __b, __c, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vmlal_s32 (int64x2_t __a, int32x2_t __b, int32x2_t __c)
{
  return (int64x2_t)__builtin_neon_vmlalv2si (__a, __b, __c, 1);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vmlal_u8 (uint16x8_t __a, uint8x8_t __b, uint8x8_t __c)
{
  return (uint16x8_t)__builtin_neon_vmlalv8qi ((int16x8_t) __a, (int8x8_t) __b, (int8x8_t) __c, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmlal_u16 (uint32x4_t __a, uint16x4_t __b, uint16x4_t __c)
{
  return (uint32x4_t)__builtin_neon_vmlalv4hi ((int32x4_t) __a, (int16x4_t) __b, (int16x4_t) __c, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vmlal_u32 (uint64x2_t __a, uint32x2_t __b, uint32x2_t __c)
{
  return (uint64x2_t)__builtin_neon_vmlalv2si ((int64x2_t) __a, (int32x2_t) __b, (int32x2_t) __c, 0);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqdmlal_s16 (int32x4_t __a, int16x4_t __b, int16x4_t __c)
{
  return (int32x4_t)__builtin_neon_vqdmlalv4hi (__a, __b, __c, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vqdmlal_s32 (int64x2_t __a, int32x2_t __b, int32x2_t __c)
{
  return (int64x2_t)__builtin_neon_vqdmlalv2si (__a, __b, __c, 1);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vmls_s8 (int8x8_t __a, int8x8_t __b, int8x8_t __c)
{
  return (int8x8_t)__builtin_neon_vmlsv8qi (__a, __b, __c, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vmls_s16 (int16x4_t __a, int16x4_t __b, int16x4_t __c)
{
  return (int16x4_t)__builtin_neon_vmlsv4hi (__a, __b, __c, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vmls_s32 (int32x2_t __a, int32x2_t __b, int32x2_t __c)
{
  return (int32x2_t)__builtin_neon_vmlsv2si (__a, __b, __c, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vmls_f32 (float32x2_t __a, float32x2_t __b, float32x2_t __c)
{
  return (float32x2_t)__builtin_neon_vmlsv2sf (__a, __b, __c, 3);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vmls_u8 (uint8x8_t __a, uint8x8_t __b, uint8x8_t __c)
{
  return (uint8x8_t)__builtin_neon_vmlsv8qi ((int8x8_t) __a, (int8x8_t) __b, (int8x8_t) __c, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vmls_u16 (uint16x4_t __a, uint16x4_t __b, uint16x4_t __c)
{
  return (uint16x4_t)__builtin_neon_vmlsv4hi ((int16x4_t) __a, (int16x4_t) __b, (int16x4_t) __c, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vmls_u32 (uint32x2_t __a, uint32x2_t __b, uint32x2_t __c)
{
  return (uint32x2_t)__builtin_neon_vmlsv2si ((int32x2_t) __a, (int32x2_t) __b, (int32x2_t) __c, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vmlsq_s8 (int8x16_t __a, int8x16_t __b, int8x16_t __c)
{
  return (int8x16_t)__builtin_neon_vmlsv16qi (__a, __b, __c, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vmlsq_s16 (int16x8_t __a, int16x8_t __b, int16x8_t __c)
{
  return (int16x8_t)__builtin_neon_vmlsv8hi (__a, __b, __c, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmlsq_s32 (int32x4_t __a, int32x4_t __b, int32x4_t __c)
{
  return (int32x4_t)__builtin_neon_vmlsv4si (__a, __b, __c, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vmlsq_f32 (float32x4_t __a, float32x4_t __b, float32x4_t __c)
{
  return (float32x4_t)__builtin_neon_vmlsv4sf (__a, __b, __c, 3);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vmlsq_u8 (uint8x16_t __a, uint8x16_t __b, uint8x16_t __c)
{
  return (uint8x16_t)__builtin_neon_vmlsv16qi ((int8x16_t) __a, (int8x16_t) __b, (int8x16_t) __c, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vmlsq_u16 (uint16x8_t __a, uint16x8_t __b, uint16x8_t __c)
{
  return (uint16x8_t)__builtin_neon_vmlsv8hi ((int16x8_t) __a, (int16x8_t) __b, (int16x8_t) __c, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmlsq_u32 (uint32x4_t __a, uint32x4_t __b, uint32x4_t __c)
{
  return (uint32x4_t)__builtin_neon_vmlsv4si ((int32x4_t) __a, (int32x4_t) __b, (int32x4_t) __c, 0);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vmlsl_s8 (int16x8_t __a, int8x8_t __b, int8x8_t __c)
{
  return (int16x8_t)__builtin_neon_vmlslv8qi (__a, __b, __c, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmlsl_s16 (int32x4_t __a, int16x4_t __b, int16x4_t __c)
{
  return (int32x4_t)__builtin_neon_vmlslv4hi (__a, __b, __c, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vmlsl_s32 (int64x2_t __a, int32x2_t __b, int32x2_t __c)
{
  return (int64x2_t)__builtin_neon_vmlslv2si (__a, __b, __c, 1);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vmlsl_u8 (uint16x8_t __a, uint8x8_t __b, uint8x8_t __c)
{
  return (uint16x8_t)__builtin_neon_vmlslv8qi ((int16x8_t) __a, (int8x8_t) __b, (int8x8_t) __c, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmlsl_u16 (uint32x4_t __a, uint16x4_t __b, uint16x4_t __c)
{
  return (uint32x4_t)__builtin_neon_vmlslv4hi ((int32x4_t) __a, (int16x4_t) __b, (int16x4_t) __c, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vmlsl_u32 (uint64x2_t __a, uint32x2_t __b, uint32x2_t __c)
{
  return (uint64x2_t)__builtin_neon_vmlslv2si ((int64x2_t) __a, (int32x2_t) __b, (int32x2_t) __c, 0);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqdmlsl_s16 (int32x4_t __a, int16x4_t __b, int16x4_t __c)
{
  return (int32x4_t)__builtin_neon_vqdmlslv4hi (__a, __b, __c, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vqdmlsl_s32 (int64x2_t __a, int32x2_t __b, int32x2_t __c)
{
  return (int64x2_t)__builtin_neon_vqdmlslv2si (__a, __b, __c, 1);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vsub_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vsubv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vsub_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vsubv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vsub_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vsubv2si (__a, __b, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vsub_f32 (float32x2_t __a, float32x2_t __b)
{
  return (float32x2_t)__builtin_neon_vsubv2sf (__a, __b, 3);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vsub_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vsubv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vsub_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vsubv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vsub_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vsubv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vsub_s64 (int64x1_t __a, int64x1_t __b)
{
  return (int64x1_t)__builtin_neon_vsubdi (__a, __b, 1);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vsub_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return (uint64x1_t)__builtin_neon_vsubdi ((int64x1_t) __a, (int64x1_t) __b, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vsubq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t)__builtin_neon_vsubv16qi (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vsubq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vsubv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vsubq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vsubv4si (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vsubq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (int64x2_t)__builtin_neon_vsubv2di (__a, __b, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vsubq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (float32x4_t)__builtin_neon_vsubv4sf (__a, __b, 3);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vsubq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vsubv16qi ((int8x16_t) __a, (int8x16_t) __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vsubq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vsubv8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vsubq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vsubv4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vsubq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return (uint64x2_t)__builtin_neon_vsubv2di ((int64x2_t) __a, (int64x2_t) __b, 0);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vsubl_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int16x8_t)__builtin_neon_vsublv8qi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vsubl_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int32x4_t)__builtin_neon_vsublv4hi (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vsubl_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int64x2_t)__builtin_neon_vsublv2si (__a, __b, 1);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vsubl_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vsublv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vsubl_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vsublv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vsubl_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint64x2_t)__builtin_neon_vsublv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vsubw_s8 (int16x8_t __a, int8x8_t __b)
{
  return (int16x8_t)__builtin_neon_vsubwv8qi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vsubw_s16 (int32x4_t __a, int16x4_t __b)
{
  return (int32x4_t)__builtin_neon_vsubwv4hi (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vsubw_s32 (int64x2_t __a, int32x2_t __b)
{
  return (int64x2_t)__builtin_neon_vsubwv2si (__a, __b, 1);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vsubw_u8 (uint16x8_t __a, uint8x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vsubwv8qi ((int16x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vsubw_u16 (uint32x4_t __a, uint16x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vsubwv4hi ((int32x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vsubw_u32 (uint64x2_t __a, uint32x2_t __b)
{
  return (uint64x2_t)__builtin_neon_vsubwv2si ((int64x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vhsub_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vhsubv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vhsub_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vhsubv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vhsub_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vhsubv2si (__a, __b, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vhsub_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vhsubv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vhsub_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vhsubv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vhsub_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vhsubv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vhsubq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t)__builtin_neon_vhsubv16qi (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vhsubq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vhsubv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vhsubq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vhsubv4si (__a, __b, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vhsubq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vhsubv16qi ((int8x16_t) __a, (int8x16_t) __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vhsubq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vhsubv8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vhsubq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vhsubv4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vqsub_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vqsubv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vqsub_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vqsubv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vqsub_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vqsubv2si (__a, __b, 1);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vqsub_s64 (int64x1_t __a, int64x1_t __b)
{
  return (int64x1_t)__builtin_neon_vqsubdi (__a, __b, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vqsub_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vqsubv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vqsub_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vqsubv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vqsub_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vqsubv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vqsub_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return (uint64x1_t)__builtin_neon_vqsubdi ((int64x1_t) __a, (int64x1_t) __b, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vqsubq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t)__builtin_neon_vqsubv16qi (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vqsubq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vqsubv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqsubq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vqsubv4si (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vqsubq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (int64x2_t)__builtin_neon_vqsubv2di (__a, __b, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vqsubq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vqsubv16qi ((int8x16_t) __a, (int8x16_t) __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vqsubq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vqsubv8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vqsubq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vqsubv4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vqsubq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return (uint64x2_t)__builtin_neon_vqsubv2di ((int64x2_t) __a, (int64x2_t) __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vsubhn_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int8x8_t)__builtin_neon_vsubhnv8hi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vsubhn_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int16x4_t)__builtin_neon_vsubhnv4si (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vsubhn_s64 (int64x2_t __a, int64x2_t __b)
{
  return (int32x2_t)__builtin_neon_vsubhnv2di (__a, __b, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vsubhn_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vsubhnv8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vsubhn_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vsubhnv4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vsubhn_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vsubhnv2di ((int64x2_t) __a, (int64x2_t) __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vrsubhn_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int8x8_t)__builtin_neon_vsubhnv8hi (__a, __b, 5);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vrsubhn_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int16x4_t)__builtin_neon_vsubhnv4si (__a, __b, 5);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vrsubhn_s64 (int64x2_t __a, int64x2_t __b)
{
  return (int32x2_t)__builtin_neon_vsubhnv2di (__a, __b, 5);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vrsubhn_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vsubhnv8hi ((int16x8_t) __a, (int16x8_t) __b, 4);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vrsubhn_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vsubhnv4si ((int32x4_t) __a, (int32x4_t) __b, 4);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vrsubhn_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vsubhnv2di ((int64x2_t) __a, (int64x2_t) __b, 4);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vceq_s8 (int8x8_t __a, int8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vceqv8qi (__a, __b, 1);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vceq_s16 (int16x4_t __a, int16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vceqv4hi (__a, __b, 1);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vceq_s32 (int32x2_t __a, int32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vceqv2si (__a, __b, 1);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vceq_f32 (float32x2_t __a, float32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vceqv2sf (__a, __b, 3);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vceq_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vceqv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vceq_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vceqv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vceq_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vceqv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vceq_p8 (poly8x8_t __a, poly8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vceqv8qi ((int8x8_t) __a, (int8x8_t) __b, 2);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vceqq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vceqv16qi (__a, __b, 1);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vceqq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vceqv8hi (__a, __b, 1);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vceqq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vceqv4si (__a, __b, 1);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vceqq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vceqv4sf (__a, __b, 3);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vceqq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vceqv16qi ((int8x16_t) __a, (int8x16_t) __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vceqq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vceqv8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vceqq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vceqv4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vceqq_p8 (poly8x16_t __a, poly8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vceqv16qi ((int8x16_t) __a, (int8x16_t) __b, 2);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vcge_s8 (int8x8_t __a, int8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vcgev8qi (__a, __b, 1);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vcge_s16 (int16x4_t __a, int16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vcgev4hi (__a, __b, 1);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vcge_s32 (int32x2_t __a, int32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vcgev2si (__a, __b, 1);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vcge_f32 (float32x2_t __a, float32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vcgev2sf (__a, __b, 3);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vcge_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vcgev8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vcge_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vcgev4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vcge_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vcgev2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vcgeq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vcgev16qi (__a, __b, 1);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vcgeq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vcgev8hi (__a, __b, 1);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vcgeq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vcgev4si (__a, __b, 1);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vcgeq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vcgev4sf (__a, __b, 3);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vcgeq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vcgev16qi ((int8x16_t) __a, (int8x16_t) __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vcgeq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vcgev8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vcgeq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vcgev4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vcle_s8 (int8x8_t __a, int8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vcgev8qi (__b, __a, 1);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vcle_s16 (int16x4_t __a, int16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vcgev4hi (__b, __a, 1);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vcle_s32 (int32x2_t __a, int32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vcgev2si (__b, __a, 1);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vcle_f32 (float32x2_t __a, float32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vcgev2sf (__b, __a, 3);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vcle_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vcgev8qi ((int8x8_t) __b, (int8x8_t) __a, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vcle_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vcgev4hi ((int16x4_t) __b, (int16x4_t) __a, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vcle_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vcgev2si ((int32x2_t) __b, (int32x2_t) __a, 0);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vcleq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vcgev16qi (__b, __a, 1);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vcleq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vcgev8hi (__b, __a, 1);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vcleq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vcgev4si (__b, __a, 1);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vcleq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vcgev4sf (__b, __a, 3);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vcleq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vcgev16qi ((int8x16_t) __b, (int8x16_t) __a, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vcleq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vcgev8hi ((int16x8_t) __b, (int16x8_t) __a, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vcleq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vcgev4si ((int32x4_t) __b, (int32x4_t) __a, 0);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vcgt_s8 (int8x8_t __a, int8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vcgtv8qi (__a, __b, 1);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vcgt_s16 (int16x4_t __a, int16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vcgtv4hi (__a, __b, 1);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vcgt_s32 (int32x2_t __a, int32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vcgtv2si (__a, __b, 1);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vcgt_f32 (float32x2_t __a, float32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vcgtv2sf (__a, __b, 3);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vcgt_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vcgtv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vcgt_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vcgtv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vcgt_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vcgtv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vcgtq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vcgtv16qi (__a, __b, 1);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vcgtq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vcgtv8hi (__a, __b, 1);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vcgtq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vcgtv4si (__a, __b, 1);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vcgtq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vcgtv4sf (__a, __b, 3);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vcgtq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vcgtv16qi ((int8x16_t) __a, (int8x16_t) __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vcgtq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vcgtv8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vcgtq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vcgtv4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vclt_s8 (int8x8_t __a, int8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vcgtv8qi (__b, __a, 1);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vclt_s16 (int16x4_t __a, int16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vcgtv4hi (__b, __a, 1);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vclt_s32 (int32x2_t __a, int32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vcgtv2si (__b, __a, 1);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vclt_f32 (float32x2_t __a, float32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vcgtv2sf (__b, __a, 3);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vclt_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vcgtv8qi ((int8x8_t) __b, (int8x8_t) __a, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vclt_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vcgtv4hi ((int16x4_t) __b, (int16x4_t) __a, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vclt_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vcgtv2si ((int32x2_t) __b, (int32x2_t) __a, 0);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vcltq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vcgtv16qi (__b, __a, 1);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vcltq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vcgtv8hi (__b, __a, 1);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vcltq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vcgtv4si (__b, __a, 1);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vcltq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vcgtv4sf (__b, __a, 3);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vcltq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vcgtv16qi ((int8x16_t) __b, (int8x16_t) __a, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vcltq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vcgtv8hi ((int16x8_t) __b, (int16x8_t) __a, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vcltq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vcgtv4si ((int32x4_t) __b, (int32x4_t) __a, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vcage_f32 (float32x2_t __a, float32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vcagev2sf (__a, __b, 3);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vcageq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vcagev4sf (__a, __b, 3);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vcale_f32 (float32x2_t __a, float32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vcagev2sf (__b, __a, 3);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vcaleq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vcagev4sf (__b, __a, 3);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vcagt_f32 (float32x2_t __a, float32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vcagtv2sf (__a, __b, 3);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vcagtq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vcagtv4sf (__a, __b, 3);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vcalt_f32 (float32x2_t __a, float32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vcagtv2sf (__b, __a, 3);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vcaltq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vcagtv4sf (__b, __a, 3);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vtst_s8 (int8x8_t __a, int8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vtstv8qi (__a, __b, 1);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vtst_s16 (int16x4_t __a, int16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vtstv4hi (__a, __b, 1);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vtst_s32 (int32x2_t __a, int32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vtstv2si (__a, __b, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vtst_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vtstv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vtst_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vtstv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vtst_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vtstv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vtst_p8 (poly8x8_t __a, poly8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vtstv8qi ((int8x8_t) __a, (int8x8_t) __b, 2);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vtstq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vtstv16qi (__a, __b, 1);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vtstq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vtstv8hi (__a, __b, 1);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vtstq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vtstv4si (__a, __b, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vtstq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vtstv16qi ((int8x16_t) __a, (int8x16_t) __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vtstq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vtstv8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vtstq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vtstv4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vtstq_p8 (poly8x16_t __a, poly8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vtstv16qi ((int8x16_t) __a, (int8x16_t) __b, 2);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vabd_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vabdv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vabd_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vabdv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vabd_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vabdv2si (__a, __b, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vabd_f32 (float32x2_t __a, float32x2_t __b)
{
  return (float32x2_t)__builtin_neon_vabdv2sf (__a, __b, 3);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vabd_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vabdv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vabd_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vabdv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vabd_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vabdv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vabdq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t)__builtin_neon_vabdv16qi (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vabdq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vabdv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vabdq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vabdv4si (__a, __b, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vabdq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (float32x4_t)__builtin_neon_vabdv4sf (__a, __b, 3);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vabdq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vabdv16qi ((int8x16_t) __a, (int8x16_t) __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vabdq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vabdv8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vabdq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vabdv4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vabdl_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int16x8_t)__builtin_neon_vabdlv8qi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vabdl_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int32x4_t)__builtin_neon_vabdlv4hi (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vabdl_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int64x2_t)__builtin_neon_vabdlv2si (__a, __b, 1);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vabdl_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vabdlv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vabdl_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vabdlv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vabdl_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint64x2_t)__builtin_neon_vabdlv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vaba_s8 (int8x8_t __a, int8x8_t __b, int8x8_t __c)
{
  return (int8x8_t)__builtin_neon_vabav8qi (__a, __b, __c, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vaba_s16 (int16x4_t __a, int16x4_t __b, int16x4_t __c)
{
  return (int16x4_t)__builtin_neon_vabav4hi (__a, __b, __c, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vaba_s32 (int32x2_t __a, int32x2_t __b, int32x2_t __c)
{
  return (int32x2_t)__builtin_neon_vabav2si (__a, __b, __c, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vaba_u8 (uint8x8_t __a, uint8x8_t __b, uint8x8_t __c)
{
  return (uint8x8_t)__builtin_neon_vabav8qi ((int8x8_t) __a, (int8x8_t) __b, (int8x8_t) __c, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vaba_u16 (uint16x4_t __a, uint16x4_t __b, uint16x4_t __c)
{
  return (uint16x4_t)__builtin_neon_vabav4hi ((int16x4_t) __a, (int16x4_t) __b, (int16x4_t) __c, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vaba_u32 (uint32x2_t __a, uint32x2_t __b, uint32x2_t __c)
{
  return (uint32x2_t)__builtin_neon_vabav2si ((int32x2_t) __a, (int32x2_t) __b, (int32x2_t) __c, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vabaq_s8 (int8x16_t __a, int8x16_t __b, int8x16_t __c)
{
  return (int8x16_t)__builtin_neon_vabav16qi (__a, __b, __c, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vabaq_s16 (int16x8_t __a, int16x8_t __b, int16x8_t __c)
{
  return (int16x8_t)__builtin_neon_vabav8hi (__a, __b, __c, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vabaq_s32 (int32x4_t __a, int32x4_t __b, int32x4_t __c)
{
  return (int32x4_t)__builtin_neon_vabav4si (__a, __b, __c, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vabaq_u8 (uint8x16_t __a, uint8x16_t __b, uint8x16_t __c)
{
  return (uint8x16_t)__builtin_neon_vabav16qi ((int8x16_t) __a, (int8x16_t) __b, (int8x16_t) __c, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vabaq_u16 (uint16x8_t __a, uint16x8_t __b, uint16x8_t __c)
{
  return (uint16x8_t)__builtin_neon_vabav8hi ((int16x8_t) __a, (int16x8_t) __b, (int16x8_t) __c, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vabaq_u32 (uint32x4_t __a, uint32x4_t __b, uint32x4_t __c)
{
  return (uint32x4_t)__builtin_neon_vabav4si ((int32x4_t) __a, (int32x4_t) __b, (int32x4_t) __c, 0);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vabal_s8 (int16x8_t __a, int8x8_t __b, int8x8_t __c)
{
  return (int16x8_t)__builtin_neon_vabalv8qi (__a, __b, __c, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vabal_s16 (int32x4_t __a, int16x4_t __b, int16x4_t __c)
{
  return (int32x4_t)__builtin_neon_vabalv4hi (__a, __b, __c, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vabal_s32 (int64x2_t __a, int32x2_t __b, int32x2_t __c)
{
  return (int64x2_t)__builtin_neon_vabalv2si (__a, __b, __c, 1);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vabal_u8 (uint16x8_t __a, uint8x8_t __b, uint8x8_t __c)
{
  return (uint16x8_t)__builtin_neon_vabalv8qi ((int16x8_t) __a, (int8x8_t) __b, (int8x8_t) __c, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vabal_u16 (uint32x4_t __a, uint16x4_t __b, uint16x4_t __c)
{
  return (uint32x4_t)__builtin_neon_vabalv4hi ((int32x4_t) __a, (int16x4_t) __b, (int16x4_t) __c, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vabal_u32 (uint64x2_t __a, uint32x2_t __b, uint32x2_t __c)
{
  return (uint64x2_t)__builtin_neon_vabalv2si ((int64x2_t) __a, (int32x2_t) __b, (int32x2_t) __c, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vmax_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vmaxv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vmax_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vmaxv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vmax_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vmaxv2si (__a, __b, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vmax_f32 (float32x2_t __a, float32x2_t __b)
{
  return (float32x2_t)__builtin_neon_vmaxv2sf (__a, __b, 3);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vmax_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vmaxv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vmax_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vmaxv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vmax_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vmaxv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vmaxq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t)__builtin_neon_vmaxv16qi (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vmaxq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vmaxv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmaxq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vmaxv4si (__a, __b, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vmaxq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (float32x4_t)__builtin_neon_vmaxv4sf (__a, __b, 3);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vmaxq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vmaxv16qi ((int8x16_t) __a, (int8x16_t) __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vmaxq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vmaxv8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmaxq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vmaxv4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vmin_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vminv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vmin_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vminv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vmin_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vminv2si (__a, __b, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vmin_f32 (float32x2_t __a, float32x2_t __b)
{
  return (float32x2_t)__builtin_neon_vminv2sf (__a, __b, 3);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vmin_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vminv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vmin_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vminv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vmin_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vminv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vminq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t)__builtin_neon_vminv16qi (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vminq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vminv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vminq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vminv4si (__a, __b, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vminq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (float32x4_t)__builtin_neon_vminv4sf (__a, __b, 3);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vminq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vminv16qi ((int8x16_t) __a, (int8x16_t) __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vminq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vminv8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vminq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vminv4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vpadd_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vpaddv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vpadd_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vpaddv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vpadd_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vpaddv2si (__a, __b, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vpadd_f32 (float32x2_t __a, float32x2_t __b)
{
  return (float32x2_t)__builtin_neon_vpaddv2sf (__a, __b, 3);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vpadd_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vpaddv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vpadd_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vpaddv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vpadd_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vpaddv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vpaddl_s8 (int8x8_t __a)
{
  return (int16x4_t)__builtin_neon_vpaddlv8qi (__a, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vpaddl_s16 (int16x4_t __a)
{
  return (int32x2_t)__builtin_neon_vpaddlv4hi (__a, 1);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vpaddl_s32 (int32x2_t __a)
{
  return (int64x1_t)__builtin_neon_vpaddlv2si (__a, 1);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vpaddl_u8 (uint8x8_t __a)
{
  return (uint16x4_t)__builtin_neon_vpaddlv8qi ((int8x8_t) __a, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vpaddl_u16 (uint16x4_t __a)
{
  return (uint32x2_t)__builtin_neon_vpaddlv4hi ((int16x4_t) __a, 0);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vpaddl_u32 (uint32x2_t __a)
{
  return (uint64x1_t)__builtin_neon_vpaddlv2si ((int32x2_t) __a, 0);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vpaddlq_s8 (int8x16_t __a)
{
  return (int16x8_t)__builtin_neon_vpaddlv16qi (__a, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vpaddlq_s16 (int16x8_t __a)
{
  return (int32x4_t)__builtin_neon_vpaddlv8hi (__a, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vpaddlq_s32 (int32x4_t __a)
{
  return (int64x2_t)__builtin_neon_vpaddlv4si (__a, 1);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vpaddlq_u8 (uint8x16_t __a)
{
  return (uint16x8_t)__builtin_neon_vpaddlv16qi ((int8x16_t) __a, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vpaddlq_u16 (uint16x8_t __a)
{
  return (uint32x4_t)__builtin_neon_vpaddlv8hi ((int16x8_t) __a, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vpaddlq_u32 (uint32x4_t __a)
{
  return (uint64x2_t)__builtin_neon_vpaddlv4si ((int32x4_t) __a, 0);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vpadal_s8 (int16x4_t __a, int8x8_t __b)
{
  return (int16x4_t)__builtin_neon_vpadalv8qi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vpadal_s16 (int32x2_t __a, int16x4_t __b)
{
  return (int32x2_t)__builtin_neon_vpadalv4hi (__a, __b, 1);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vpadal_s32 (int64x1_t __a, int32x2_t __b)
{
  return (int64x1_t)__builtin_neon_vpadalv2si (__a, __b, 1);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vpadal_u8 (uint16x4_t __a, uint8x8_t __b)
{
  return (uint16x4_t)__builtin_neon_vpadalv8qi ((int16x4_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vpadal_u16 (uint32x2_t __a, uint16x4_t __b)
{
  return (uint32x2_t)__builtin_neon_vpadalv4hi ((int32x2_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vpadal_u32 (uint64x1_t __a, uint32x2_t __b)
{
  return (uint64x1_t)__builtin_neon_vpadalv2si ((int64x1_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vpadalq_s8 (int16x8_t __a, int8x16_t __b)
{
  return (int16x8_t)__builtin_neon_vpadalv16qi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vpadalq_s16 (int32x4_t __a, int16x8_t __b)
{
  return (int32x4_t)__builtin_neon_vpadalv8hi (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vpadalq_s32 (int64x2_t __a, int32x4_t __b)
{
  return (int64x2_t)__builtin_neon_vpadalv4si (__a, __b, 1);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vpadalq_u8 (uint16x8_t __a, uint8x16_t __b)
{
  return (uint16x8_t)__builtin_neon_vpadalv16qi ((int16x8_t) __a, (int8x16_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vpadalq_u16 (uint32x4_t __a, uint16x8_t __b)
{
  return (uint32x4_t)__builtin_neon_vpadalv8hi ((int32x4_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vpadalq_u32 (uint64x2_t __a, uint32x4_t __b)
{
  return (uint64x2_t)__builtin_neon_vpadalv4si ((int64x2_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vpmax_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vpmaxv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vpmax_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vpmaxv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vpmax_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vpmaxv2si (__a, __b, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vpmax_f32 (float32x2_t __a, float32x2_t __b)
{
  return (float32x2_t)__builtin_neon_vpmaxv2sf (__a, __b, 3);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vpmax_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vpmaxv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vpmax_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vpmaxv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vpmax_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vpmaxv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vpmin_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vpminv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vpmin_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vpminv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vpmin_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vpminv2si (__a, __b, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vpmin_f32 (float32x2_t __a, float32x2_t __b)
{
  return (float32x2_t)__builtin_neon_vpminv2sf (__a, __b, 3);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vpmin_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vpminv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vpmin_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vpminv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vpmin_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vpminv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vrecps_f32 (float32x2_t __a, float32x2_t __b)
{
  return (float32x2_t)__builtin_neon_vrecpsv2sf (__a, __b, 3);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vrecpsq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (float32x4_t)__builtin_neon_vrecpsv4sf (__a, __b, 3);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vrsqrts_f32 (float32x2_t __a, float32x2_t __b)
{
  return (float32x2_t)__builtin_neon_vrsqrtsv2sf (__a, __b, 3);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vrsqrtsq_f32 (float32x4_t __a, float32x4_t __b)
{
  return (float32x4_t)__builtin_neon_vrsqrtsv4sf (__a, __b, 3);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vshl_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vshlv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vshl_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vshlv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vshl_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vshlv2si (__a, __b, 1);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vshl_s64 (int64x1_t __a, int64x1_t __b)
{
  return (int64x1_t)__builtin_neon_vshldi (__a, __b, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vshl_u8 (uint8x8_t __a, int8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vshlv8qi ((int8x8_t) __a, __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vshl_u16 (uint16x4_t __a, int16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vshlv4hi ((int16x4_t) __a, __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vshl_u32 (uint32x2_t __a, int32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vshlv2si ((int32x2_t) __a, __b, 0);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vshl_u64 (uint64x1_t __a, int64x1_t __b)
{
  return (uint64x1_t)__builtin_neon_vshldi ((int64x1_t) __a, __b, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vshlq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t)__builtin_neon_vshlv16qi (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vshlq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vshlv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vshlq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vshlv4si (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vshlq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (int64x2_t)__builtin_neon_vshlv2di (__a, __b, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vshlq_u8 (uint8x16_t __a, int8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vshlv16qi ((int8x16_t) __a, __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vshlq_u16 (uint16x8_t __a, int16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vshlv8hi ((int16x8_t) __a, __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vshlq_u32 (uint32x4_t __a, int32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vshlv4si ((int32x4_t) __a, __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vshlq_u64 (uint64x2_t __a, int64x2_t __b)
{
  return (uint64x2_t)__builtin_neon_vshlv2di ((int64x2_t) __a, __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vrshl_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vshlv8qi (__a, __b, 5);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vrshl_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vshlv4hi (__a, __b, 5);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vrshl_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vshlv2si (__a, __b, 5);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vrshl_s64 (int64x1_t __a, int64x1_t __b)
{
  return (int64x1_t)__builtin_neon_vshldi (__a, __b, 5);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vrshl_u8 (uint8x8_t __a, int8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vshlv8qi ((int8x8_t) __a, __b, 4);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vrshl_u16 (uint16x4_t __a, int16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vshlv4hi ((int16x4_t) __a, __b, 4);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vrshl_u32 (uint32x2_t __a, int32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vshlv2si ((int32x2_t) __a, __b, 4);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vrshl_u64 (uint64x1_t __a, int64x1_t __b)
{
  return (uint64x1_t)__builtin_neon_vshldi ((int64x1_t) __a, __b, 4);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vrshlq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t)__builtin_neon_vshlv16qi (__a, __b, 5);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vrshlq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vshlv8hi (__a, __b, 5);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vrshlq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vshlv4si (__a, __b, 5);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vrshlq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (int64x2_t)__builtin_neon_vshlv2di (__a, __b, 5);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vrshlq_u8 (uint8x16_t __a, int8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vshlv16qi ((int8x16_t) __a, __b, 4);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vrshlq_u16 (uint16x8_t __a, int16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vshlv8hi ((int16x8_t) __a, __b, 4);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vrshlq_u32 (uint32x4_t __a, int32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vshlv4si ((int32x4_t) __a, __b, 4);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vrshlq_u64 (uint64x2_t __a, int64x2_t __b)
{
  return (uint64x2_t)__builtin_neon_vshlv2di ((int64x2_t) __a, __b, 4);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vqshl_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vqshlv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vqshl_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vqshlv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vqshl_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vqshlv2si (__a, __b, 1);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vqshl_s64 (int64x1_t __a, int64x1_t __b)
{
  return (int64x1_t)__builtin_neon_vqshldi (__a, __b, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vqshl_u8 (uint8x8_t __a, int8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vqshlv8qi ((int8x8_t) __a, __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vqshl_u16 (uint16x4_t __a, int16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vqshlv4hi ((int16x4_t) __a, __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vqshl_u32 (uint32x2_t __a, int32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vqshlv2si ((int32x2_t) __a, __b, 0);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vqshl_u64 (uint64x1_t __a, int64x1_t __b)
{
  return (uint64x1_t)__builtin_neon_vqshldi ((int64x1_t) __a, __b, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vqshlq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t)__builtin_neon_vqshlv16qi (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vqshlq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vqshlv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqshlq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vqshlv4si (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vqshlq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (int64x2_t)__builtin_neon_vqshlv2di (__a, __b, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vqshlq_u8 (uint8x16_t __a, int8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vqshlv16qi ((int8x16_t) __a, __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vqshlq_u16 (uint16x8_t __a, int16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vqshlv8hi ((int16x8_t) __a, __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vqshlq_u32 (uint32x4_t __a, int32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vqshlv4si ((int32x4_t) __a, __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vqshlq_u64 (uint64x2_t __a, int64x2_t __b)
{
  return (uint64x2_t)__builtin_neon_vqshlv2di ((int64x2_t) __a, __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vqrshl_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vqshlv8qi (__a, __b, 5);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vqrshl_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vqshlv4hi (__a, __b, 5);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vqrshl_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vqshlv2si (__a, __b, 5);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vqrshl_s64 (int64x1_t __a, int64x1_t __b)
{
  return (int64x1_t)__builtin_neon_vqshldi (__a, __b, 5);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vqrshl_u8 (uint8x8_t __a, int8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vqshlv8qi ((int8x8_t) __a, __b, 4);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vqrshl_u16 (uint16x4_t __a, int16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vqshlv4hi ((int16x4_t) __a, __b, 4);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vqrshl_u32 (uint32x2_t __a, int32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vqshlv2si ((int32x2_t) __a, __b, 4);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vqrshl_u64 (uint64x1_t __a, int64x1_t __b)
{
  return (uint64x1_t)__builtin_neon_vqshldi ((int64x1_t) __a, __b, 4);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vqrshlq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t)__builtin_neon_vqshlv16qi (__a, __b, 5);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vqrshlq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vqshlv8hi (__a, __b, 5);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqrshlq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vqshlv4si (__a, __b, 5);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vqrshlq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (int64x2_t)__builtin_neon_vqshlv2di (__a, __b, 5);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vqrshlq_u8 (uint8x16_t __a, int8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vqshlv16qi ((int8x16_t) __a, __b, 4);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vqrshlq_u16 (uint16x8_t __a, int16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vqshlv8hi ((int16x8_t) __a, __b, 4);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vqrshlq_u32 (uint32x4_t __a, int32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vqshlv4si ((int32x4_t) __a, __b, 4);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vqrshlq_u64 (uint64x2_t __a, int64x2_t __b)
{
  return (uint64x2_t)__builtin_neon_vqshlv2di ((int64x2_t) __a, __b, 4);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vshr_n_s8 (int8x8_t __a, const int __b)
{
  return (int8x8_t)__builtin_neon_vshr_nv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vshr_n_s16 (int16x4_t __a, const int __b)
{
  return (int16x4_t)__builtin_neon_vshr_nv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vshr_n_s32 (int32x2_t __a, const int __b)
{
  return (int32x2_t)__builtin_neon_vshr_nv2si (__a, __b, 1);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vshr_n_s64 (int64x1_t __a, const int __b)
{
  return (int64x1_t)__builtin_neon_vshr_ndi (__a, __b, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vshr_n_u8 (uint8x8_t __a, const int __b)
{
  return (uint8x8_t)__builtin_neon_vshr_nv8qi ((int8x8_t) __a, __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vshr_n_u16 (uint16x4_t __a, const int __b)
{
  return (uint16x4_t)__builtin_neon_vshr_nv4hi ((int16x4_t) __a, __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vshr_n_u32 (uint32x2_t __a, const int __b)
{
  return (uint32x2_t)__builtin_neon_vshr_nv2si ((int32x2_t) __a, __b, 0);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vshr_n_u64 (uint64x1_t __a, const int __b)
{
  return (uint64x1_t)__builtin_neon_vshr_ndi ((int64x1_t) __a, __b, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vshrq_n_s8 (int8x16_t __a, const int __b)
{
  return (int8x16_t)__builtin_neon_vshr_nv16qi (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vshrq_n_s16 (int16x8_t __a, const int __b)
{
  return (int16x8_t)__builtin_neon_vshr_nv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vshrq_n_s32 (int32x4_t __a, const int __b)
{
  return (int32x4_t)__builtin_neon_vshr_nv4si (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vshrq_n_s64 (int64x2_t __a, const int __b)
{
  return (int64x2_t)__builtin_neon_vshr_nv2di (__a, __b, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vshrq_n_u8 (uint8x16_t __a, const int __b)
{
  return (uint8x16_t)__builtin_neon_vshr_nv16qi ((int8x16_t) __a, __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vshrq_n_u16 (uint16x8_t __a, const int __b)
{
  return (uint16x8_t)__builtin_neon_vshr_nv8hi ((int16x8_t) __a, __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vshrq_n_u32 (uint32x4_t __a, const int __b)
{
  return (uint32x4_t)__builtin_neon_vshr_nv4si ((int32x4_t) __a, __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vshrq_n_u64 (uint64x2_t __a, const int __b)
{
  return (uint64x2_t)__builtin_neon_vshr_nv2di ((int64x2_t) __a, __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vrshr_n_s8 (int8x8_t __a, const int __b)
{
  return (int8x8_t)__builtin_neon_vshr_nv8qi (__a, __b, 5);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vrshr_n_s16 (int16x4_t __a, const int __b)
{
  return (int16x4_t)__builtin_neon_vshr_nv4hi (__a, __b, 5);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vrshr_n_s32 (int32x2_t __a, const int __b)
{
  return (int32x2_t)__builtin_neon_vshr_nv2si (__a, __b, 5);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vrshr_n_s64 (int64x1_t __a, const int __b)
{
  return (int64x1_t)__builtin_neon_vshr_ndi (__a, __b, 5);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vrshr_n_u8 (uint8x8_t __a, const int __b)
{
  return (uint8x8_t)__builtin_neon_vshr_nv8qi ((int8x8_t) __a, __b, 4);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vrshr_n_u16 (uint16x4_t __a, const int __b)
{
  return (uint16x4_t)__builtin_neon_vshr_nv4hi ((int16x4_t) __a, __b, 4);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vrshr_n_u32 (uint32x2_t __a, const int __b)
{
  return (uint32x2_t)__builtin_neon_vshr_nv2si ((int32x2_t) __a, __b, 4);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vrshr_n_u64 (uint64x1_t __a, const int __b)
{
  return (uint64x1_t)__builtin_neon_vshr_ndi ((int64x1_t) __a, __b, 4);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vrshrq_n_s8 (int8x16_t __a, const int __b)
{
  return (int8x16_t)__builtin_neon_vshr_nv16qi (__a, __b, 5);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vrshrq_n_s16 (int16x8_t __a, const int __b)
{
  return (int16x8_t)__builtin_neon_vshr_nv8hi (__a, __b, 5);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vrshrq_n_s32 (int32x4_t __a, const int __b)
{
  return (int32x4_t)__builtin_neon_vshr_nv4si (__a, __b, 5);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vrshrq_n_s64 (int64x2_t __a, const int __b)
{
  return (int64x2_t)__builtin_neon_vshr_nv2di (__a, __b, 5);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vrshrq_n_u8 (uint8x16_t __a, const int __b)
{
  return (uint8x16_t)__builtin_neon_vshr_nv16qi ((int8x16_t) __a, __b, 4);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vrshrq_n_u16 (uint16x8_t __a, const int __b)
{
  return (uint16x8_t)__builtin_neon_vshr_nv8hi ((int16x8_t) __a, __b, 4);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vrshrq_n_u32 (uint32x4_t __a, const int __b)
{
  return (uint32x4_t)__builtin_neon_vshr_nv4si ((int32x4_t) __a, __b, 4);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vrshrq_n_u64 (uint64x2_t __a, const int __b)
{
  return (uint64x2_t)__builtin_neon_vshr_nv2di ((int64x2_t) __a, __b, 4);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vshrn_n_s16 (int16x8_t __a, const int __b)
{
  return (int8x8_t)__builtin_neon_vshrn_nv8hi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vshrn_n_s32 (int32x4_t __a, const int __b)
{
  return (int16x4_t)__builtin_neon_vshrn_nv4si (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vshrn_n_s64 (int64x2_t __a, const int __b)
{
  return (int32x2_t)__builtin_neon_vshrn_nv2di (__a, __b, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vshrn_n_u16 (uint16x8_t __a, const int __b)
{
  return (uint8x8_t)__builtin_neon_vshrn_nv8hi ((int16x8_t) __a, __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vshrn_n_u32 (uint32x4_t __a, const int __b)
{
  return (uint16x4_t)__builtin_neon_vshrn_nv4si ((int32x4_t) __a, __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vshrn_n_u64 (uint64x2_t __a, const int __b)
{
  return (uint32x2_t)__builtin_neon_vshrn_nv2di ((int64x2_t) __a, __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vrshrn_n_s16 (int16x8_t __a, const int __b)
{
  return (int8x8_t)__builtin_neon_vshrn_nv8hi (__a, __b, 5);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vrshrn_n_s32 (int32x4_t __a, const int __b)
{
  return (int16x4_t)__builtin_neon_vshrn_nv4si (__a, __b, 5);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vrshrn_n_s64 (int64x2_t __a, const int __b)
{
  return (int32x2_t)__builtin_neon_vshrn_nv2di (__a, __b, 5);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vrshrn_n_u16 (uint16x8_t __a, const int __b)
{
  return (uint8x8_t)__builtin_neon_vshrn_nv8hi ((int16x8_t) __a, __b, 4);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vrshrn_n_u32 (uint32x4_t __a, const int __b)
{
  return (uint16x4_t)__builtin_neon_vshrn_nv4si ((int32x4_t) __a, __b, 4);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vrshrn_n_u64 (uint64x2_t __a, const int __b)
{
  return (uint32x2_t)__builtin_neon_vshrn_nv2di ((int64x2_t) __a, __b, 4);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vqshrn_n_s16 (int16x8_t __a, const int __b)
{
  return (int8x8_t)__builtin_neon_vqshrn_nv8hi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vqshrn_n_s32 (int32x4_t __a, const int __b)
{
  return (int16x4_t)__builtin_neon_vqshrn_nv4si (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vqshrn_n_s64 (int64x2_t __a, const int __b)
{
  return (int32x2_t)__builtin_neon_vqshrn_nv2di (__a, __b, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vqshrn_n_u16 (uint16x8_t __a, const int __b)
{
  return (uint8x8_t)__builtin_neon_vqshrn_nv8hi ((int16x8_t) __a, __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vqshrn_n_u32 (uint32x4_t __a, const int __b)
{
  return (uint16x4_t)__builtin_neon_vqshrn_nv4si ((int32x4_t) __a, __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vqshrn_n_u64 (uint64x2_t __a, const int __b)
{
  return (uint32x2_t)__builtin_neon_vqshrn_nv2di ((int64x2_t) __a, __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vqrshrn_n_s16 (int16x8_t __a, const int __b)
{
  return (int8x8_t)__builtin_neon_vqshrn_nv8hi (__a, __b, 5);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vqrshrn_n_s32 (int32x4_t __a, const int __b)
{
  return (int16x4_t)__builtin_neon_vqshrn_nv4si (__a, __b, 5);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vqrshrn_n_s64 (int64x2_t __a, const int __b)
{
  return (int32x2_t)__builtin_neon_vqshrn_nv2di (__a, __b, 5);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vqrshrn_n_u16 (uint16x8_t __a, const int __b)
{
  return (uint8x8_t)__builtin_neon_vqshrn_nv8hi ((int16x8_t) __a, __b, 4);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vqrshrn_n_u32 (uint32x4_t __a, const int __b)
{
  return (uint16x4_t)__builtin_neon_vqshrn_nv4si ((int32x4_t) __a, __b, 4);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vqrshrn_n_u64 (uint64x2_t __a, const int __b)
{
  return (uint32x2_t)__builtin_neon_vqshrn_nv2di ((int64x2_t) __a, __b, 4);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vqshrun_n_s16 (int16x8_t __a, const int __b)
{
  return (uint8x8_t)__builtin_neon_vqshrun_nv8hi (__a, __b, 1);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vqshrun_n_s32 (int32x4_t __a, const int __b)
{
  return (uint16x4_t)__builtin_neon_vqshrun_nv4si (__a, __b, 1);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vqshrun_n_s64 (int64x2_t __a, const int __b)
{
  return (uint32x2_t)__builtin_neon_vqshrun_nv2di (__a, __b, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vqrshrun_n_s16 (int16x8_t __a, const int __b)
{
  return (uint8x8_t)__builtin_neon_vqshrun_nv8hi (__a, __b, 5);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vqrshrun_n_s32 (int32x4_t __a, const int __b)
{
  return (uint16x4_t)__builtin_neon_vqshrun_nv4si (__a, __b, 5);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vqrshrun_n_s64 (int64x2_t __a, const int __b)
{
  return (uint32x2_t)__builtin_neon_vqshrun_nv2di (__a, __b, 5);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vshl_n_s8 (int8x8_t __a, const int __b)
{
  return (int8x8_t)__builtin_neon_vshl_nv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vshl_n_s16 (int16x4_t __a, const int __b)
{
  return (int16x4_t)__builtin_neon_vshl_nv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vshl_n_s32 (int32x2_t __a, const int __b)
{
  return (int32x2_t)__builtin_neon_vshl_nv2si (__a, __b, 1);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vshl_n_s64 (int64x1_t __a, const int __b)
{
  return (int64x1_t)__builtin_neon_vshl_ndi (__a, __b, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vshl_n_u8 (uint8x8_t __a, const int __b)
{
  return (uint8x8_t)__builtin_neon_vshl_nv8qi ((int8x8_t) __a, __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vshl_n_u16 (uint16x4_t __a, const int __b)
{
  return (uint16x4_t)__builtin_neon_vshl_nv4hi ((int16x4_t) __a, __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vshl_n_u32 (uint32x2_t __a, const int __b)
{
  return (uint32x2_t)__builtin_neon_vshl_nv2si ((int32x2_t) __a, __b, 0);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vshl_n_u64 (uint64x1_t __a, const int __b)
{
  return (uint64x1_t)__builtin_neon_vshl_ndi ((int64x1_t) __a, __b, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vshlq_n_s8 (int8x16_t __a, const int __b)
{
  return (int8x16_t)__builtin_neon_vshl_nv16qi (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vshlq_n_s16 (int16x8_t __a, const int __b)
{
  return (int16x8_t)__builtin_neon_vshl_nv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vshlq_n_s32 (int32x4_t __a, const int __b)
{
  return (int32x4_t)__builtin_neon_vshl_nv4si (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vshlq_n_s64 (int64x2_t __a, const int __b)
{
  return (int64x2_t)__builtin_neon_vshl_nv2di (__a, __b, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vshlq_n_u8 (uint8x16_t __a, const int __b)
{
  return (uint8x16_t)__builtin_neon_vshl_nv16qi ((int8x16_t) __a, __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vshlq_n_u16 (uint16x8_t __a, const int __b)
{
  return (uint16x8_t)__builtin_neon_vshl_nv8hi ((int16x8_t) __a, __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vshlq_n_u32 (uint32x4_t __a, const int __b)
{
  return (uint32x4_t)__builtin_neon_vshl_nv4si ((int32x4_t) __a, __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vshlq_n_u64 (uint64x2_t __a, const int __b)
{
  return (uint64x2_t)__builtin_neon_vshl_nv2di ((int64x2_t) __a, __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vqshl_n_s8 (int8x8_t __a, const int __b)
{
  return (int8x8_t)__builtin_neon_vqshl_nv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vqshl_n_s16 (int16x4_t __a, const int __b)
{
  return (int16x4_t)__builtin_neon_vqshl_nv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vqshl_n_s32 (int32x2_t __a, const int __b)
{
  return (int32x2_t)__builtin_neon_vqshl_nv2si (__a, __b, 1);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vqshl_n_s64 (int64x1_t __a, const int __b)
{
  return (int64x1_t)__builtin_neon_vqshl_ndi (__a, __b, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vqshl_n_u8 (uint8x8_t __a, const int __b)
{
  return (uint8x8_t)__builtin_neon_vqshl_nv8qi ((int8x8_t) __a, __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vqshl_n_u16 (uint16x4_t __a, const int __b)
{
  return (uint16x4_t)__builtin_neon_vqshl_nv4hi ((int16x4_t) __a, __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vqshl_n_u32 (uint32x2_t __a, const int __b)
{
  return (uint32x2_t)__builtin_neon_vqshl_nv2si ((int32x2_t) __a, __b, 0);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vqshl_n_u64 (uint64x1_t __a, const int __b)
{
  return (uint64x1_t)__builtin_neon_vqshl_ndi ((int64x1_t) __a, __b, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vqshlq_n_s8 (int8x16_t __a, const int __b)
{
  return (int8x16_t)__builtin_neon_vqshl_nv16qi (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vqshlq_n_s16 (int16x8_t __a, const int __b)
{
  return (int16x8_t)__builtin_neon_vqshl_nv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqshlq_n_s32 (int32x4_t __a, const int __b)
{
  return (int32x4_t)__builtin_neon_vqshl_nv4si (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vqshlq_n_s64 (int64x2_t __a, const int __b)
{
  return (int64x2_t)__builtin_neon_vqshl_nv2di (__a, __b, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vqshlq_n_u8 (uint8x16_t __a, const int __b)
{
  return (uint8x16_t)__builtin_neon_vqshl_nv16qi ((int8x16_t) __a, __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vqshlq_n_u16 (uint16x8_t __a, const int __b)
{
  return (uint16x8_t)__builtin_neon_vqshl_nv8hi ((int16x8_t) __a, __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vqshlq_n_u32 (uint32x4_t __a, const int __b)
{
  return (uint32x4_t)__builtin_neon_vqshl_nv4si ((int32x4_t) __a, __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vqshlq_n_u64 (uint64x2_t __a, const int __b)
{
  return (uint64x2_t)__builtin_neon_vqshl_nv2di ((int64x2_t) __a, __b, 0);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vqshlu_n_s8 (int8x8_t __a, const int __b)
{
  return (uint8x8_t)__builtin_neon_vqshlu_nv8qi (__a, __b, 1);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vqshlu_n_s16 (int16x4_t __a, const int __b)
{
  return (uint16x4_t)__builtin_neon_vqshlu_nv4hi (__a, __b, 1);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vqshlu_n_s32 (int32x2_t __a, const int __b)
{
  return (uint32x2_t)__builtin_neon_vqshlu_nv2si (__a, __b, 1);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vqshlu_n_s64 (int64x1_t __a, const int __b)
{
  return (uint64x1_t)__builtin_neon_vqshlu_ndi (__a, __b, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vqshluq_n_s8 (int8x16_t __a, const int __b)
{
  return (uint8x16_t)__builtin_neon_vqshlu_nv16qi (__a, __b, 1);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vqshluq_n_s16 (int16x8_t __a, const int __b)
{
  return (uint16x8_t)__builtin_neon_vqshlu_nv8hi (__a, __b, 1);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vqshluq_n_s32 (int32x4_t __a, const int __b)
{
  return (uint32x4_t)__builtin_neon_vqshlu_nv4si (__a, __b, 1);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vqshluq_n_s64 (int64x2_t __a, const int __b)
{
  return (uint64x2_t)__builtin_neon_vqshlu_nv2di (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vshll_n_s8 (int8x8_t __a, const int __b)
{
  return (int16x8_t)__builtin_neon_vshll_nv8qi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vshll_n_s16 (int16x4_t __a, const int __b)
{
  return (int32x4_t)__builtin_neon_vshll_nv4hi (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vshll_n_s32 (int32x2_t __a, const int __b)
{
  return (int64x2_t)__builtin_neon_vshll_nv2si (__a, __b, 1);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vshll_n_u8 (uint8x8_t __a, const int __b)
{
  return (uint16x8_t)__builtin_neon_vshll_nv8qi ((int8x8_t) __a, __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vshll_n_u16 (uint16x4_t __a, const int __b)
{
  return (uint32x4_t)__builtin_neon_vshll_nv4hi ((int16x4_t) __a, __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vshll_n_u32 (uint32x2_t __a, const int __b)
{
  return (uint64x2_t)__builtin_neon_vshll_nv2si ((int32x2_t) __a, __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vsra_n_s8 (int8x8_t __a, int8x8_t __b, const int __c)
{
  return (int8x8_t)__builtin_neon_vsra_nv8qi (__a, __b, __c, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vsra_n_s16 (int16x4_t __a, int16x4_t __b, const int __c)
{
  return (int16x4_t)__builtin_neon_vsra_nv4hi (__a, __b, __c, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vsra_n_s32 (int32x2_t __a, int32x2_t __b, const int __c)
{
  return (int32x2_t)__builtin_neon_vsra_nv2si (__a, __b, __c, 1);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vsra_n_s64 (int64x1_t __a, int64x1_t __b, const int __c)
{
  return (int64x1_t)__builtin_neon_vsra_ndi (__a, __b, __c, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vsra_n_u8 (uint8x8_t __a, uint8x8_t __b, const int __c)
{
  return (uint8x8_t)__builtin_neon_vsra_nv8qi ((int8x8_t) __a, (int8x8_t) __b, __c, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vsra_n_u16 (uint16x4_t __a, uint16x4_t __b, const int __c)
{
  return (uint16x4_t)__builtin_neon_vsra_nv4hi ((int16x4_t) __a, (int16x4_t) __b, __c, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vsra_n_u32 (uint32x2_t __a, uint32x2_t __b, const int __c)
{
  return (uint32x2_t)__builtin_neon_vsra_nv2si ((int32x2_t) __a, (int32x2_t) __b, __c, 0);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vsra_n_u64 (uint64x1_t __a, uint64x1_t __b, const int __c)
{
  return (uint64x1_t)__builtin_neon_vsra_ndi ((int64x1_t) __a, (int64x1_t) __b, __c, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vsraq_n_s8 (int8x16_t __a, int8x16_t __b, const int __c)
{
  return (int8x16_t)__builtin_neon_vsra_nv16qi (__a, __b, __c, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vsraq_n_s16 (int16x8_t __a, int16x8_t __b, const int __c)
{
  return (int16x8_t)__builtin_neon_vsra_nv8hi (__a, __b, __c, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vsraq_n_s32 (int32x4_t __a, int32x4_t __b, const int __c)
{
  return (int32x4_t)__builtin_neon_vsra_nv4si (__a, __b, __c, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vsraq_n_s64 (int64x2_t __a, int64x2_t __b, const int __c)
{
  return (int64x2_t)__builtin_neon_vsra_nv2di (__a, __b, __c, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vsraq_n_u8 (uint8x16_t __a, uint8x16_t __b, const int __c)
{
  return (uint8x16_t)__builtin_neon_vsra_nv16qi ((int8x16_t) __a, (int8x16_t) __b, __c, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vsraq_n_u16 (uint16x8_t __a, uint16x8_t __b, const int __c)
{
  return (uint16x8_t)__builtin_neon_vsra_nv8hi ((int16x8_t) __a, (int16x8_t) __b, __c, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vsraq_n_u32 (uint32x4_t __a, uint32x4_t __b, const int __c)
{
  return (uint32x4_t)__builtin_neon_vsra_nv4si ((int32x4_t) __a, (int32x4_t) __b, __c, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vsraq_n_u64 (uint64x2_t __a, uint64x2_t __b, const int __c)
{
  return (uint64x2_t)__builtin_neon_vsra_nv2di ((int64x2_t) __a, (int64x2_t) __b, __c, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vrsra_n_s8 (int8x8_t __a, int8x8_t __b, const int __c)
{
  return (int8x8_t)__builtin_neon_vsra_nv8qi (__a, __b, __c, 5);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vrsra_n_s16 (int16x4_t __a, int16x4_t __b, const int __c)
{
  return (int16x4_t)__builtin_neon_vsra_nv4hi (__a, __b, __c, 5);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vrsra_n_s32 (int32x2_t __a, int32x2_t __b, const int __c)
{
  return (int32x2_t)__builtin_neon_vsra_nv2si (__a, __b, __c, 5);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vrsra_n_s64 (int64x1_t __a, int64x1_t __b, const int __c)
{
  return (int64x1_t)__builtin_neon_vsra_ndi (__a, __b, __c, 5);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vrsra_n_u8 (uint8x8_t __a, uint8x8_t __b, const int __c)
{
  return (uint8x8_t)__builtin_neon_vsra_nv8qi ((int8x8_t) __a, (int8x8_t) __b, __c, 4);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vrsra_n_u16 (uint16x4_t __a, uint16x4_t __b, const int __c)
{
  return (uint16x4_t)__builtin_neon_vsra_nv4hi ((int16x4_t) __a, (int16x4_t) __b, __c, 4);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vrsra_n_u32 (uint32x2_t __a, uint32x2_t __b, const int __c)
{
  return (uint32x2_t)__builtin_neon_vsra_nv2si ((int32x2_t) __a, (int32x2_t) __b, __c, 4);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vrsra_n_u64 (uint64x1_t __a, uint64x1_t __b, const int __c)
{
  return (uint64x1_t)__builtin_neon_vsra_ndi ((int64x1_t) __a, (int64x1_t) __b, __c, 4);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vrsraq_n_s8 (int8x16_t __a, int8x16_t __b, const int __c)
{
  return (int8x16_t)__builtin_neon_vsra_nv16qi (__a, __b, __c, 5);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vrsraq_n_s16 (int16x8_t __a, int16x8_t __b, const int __c)
{
  return (int16x8_t)__builtin_neon_vsra_nv8hi (__a, __b, __c, 5);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vrsraq_n_s32 (int32x4_t __a, int32x4_t __b, const int __c)
{
  return (int32x4_t)__builtin_neon_vsra_nv4si (__a, __b, __c, 5);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vrsraq_n_s64 (int64x2_t __a, int64x2_t __b, const int __c)
{
  return (int64x2_t)__builtin_neon_vsra_nv2di (__a, __b, __c, 5);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vrsraq_n_u8 (uint8x16_t __a, uint8x16_t __b, const int __c)
{
  return (uint8x16_t)__builtin_neon_vsra_nv16qi ((int8x16_t) __a, (int8x16_t) __b, __c, 4);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vrsraq_n_u16 (uint16x8_t __a, uint16x8_t __b, const int __c)
{
  return (uint16x8_t)__builtin_neon_vsra_nv8hi ((int16x8_t) __a, (int16x8_t) __b, __c, 4);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vrsraq_n_u32 (uint32x4_t __a, uint32x4_t __b, const int __c)
{
  return (uint32x4_t)__builtin_neon_vsra_nv4si ((int32x4_t) __a, (int32x4_t) __b, __c, 4);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vrsraq_n_u64 (uint64x2_t __a, uint64x2_t __b, const int __c)
{
  return (uint64x2_t)__builtin_neon_vsra_nv2di ((int64x2_t) __a, (int64x2_t) __b, __c, 4);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vsri_n_s8 (int8x8_t __a, int8x8_t __b, const int __c)
{
  return (int8x8_t)__builtin_neon_vsri_nv8qi (__a, __b, __c);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vsri_n_s16 (int16x4_t __a, int16x4_t __b, const int __c)
{
  return (int16x4_t)__builtin_neon_vsri_nv4hi (__a, __b, __c);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vsri_n_s32 (int32x2_t __a, int32x2_t __b, const int __c)
{
  return (int32x2_t)__builtin_neon_vsri_nv2si (__a, __b, __c);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vsri_n_s64 (int64x1_t __a, int64x1_t __b, const int __c)
{
  return (int64x1_t)__builtin_neon_vsri_ndi (__a, __b, __c);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vsri_n_u8 (uint8x8_t __a, uint8x8_t __b, const int __c)
{
  return (uint8x8_t)__builtin_neon_vsri_nv8qi ((int8x8_t) __a, (int8x8_t) __b, __c);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vsri_n_u16 (uint16x4_t __a, uint16x4_t __b, const int __c)
{
  return (uint16x4_t)__builtin_neon_vsri_nv4hi ((int16x4_t) __a, (int16x4_t) __b, __c);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vsri_n_u32 (uint32x2_t __a, uint32x2_t __b, const int __c)
{
  return (uint32x2_t)__builtin_neon_vsri_nv2si ((int32x2_t) __a, (int32x2_t) __b, __c);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vsri_n_u64 (uint64x1_t __a, uint64x1_t __b, const int __c)
{
  return (uint64x1_t)__builtin_neon_vsri_ndi ((int64x1_t) __a, (int64x1_t) __b, __c);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vsri_n_p8 (poly8x8_t __a, poly8x8_t __b, const int __c)
{
  return (poly8x8_t)__builtin_neon_vsri_nv8qi ((int8x8_t) __a, (int8x8_t) __b, __c);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vsri_n_p16 (poly16x4_t __a, poly16x4_t __b, const int __c)
{
  return (poly16x4_t)__builtin_neon_vsri_nv4hi ((int16x4_t) __a, (int16x4_t) __b, __c);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vsriq_n_s8 (int8x16_t __a, int8x16_t __b, const int __c)
{
  return (int8x16_t)__builtin_neon_vsri_nv16qi (__a, __b, __c);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vsriq_n_s16 (int16x8_t __a, int16x8_t __b, const int __c)
{
  return (int16x8_t)__builtin_neon_vsri_nv8hi (__a, __b, __c);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vsriq_n_s32 (int32x4_t __a, int32x4_t __b, const int __c)
{
  return (int32x4_t)__builtin_neon_vsri_nv4si (__a, __b, __c);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vsriq_n_s64 (int64x2_t __a, int64x2_t __b, const int __c)
{
  return (int64x2_t)__builtin_neon_vsri_nv2di (__a, __b, __c);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vsriq_n_u8 (uint8x16_t __a, uint8x16_t __b, const int __c)
{
  return (uint8x16_t)__builtin_neon_vsri_nv16qi ((int8x16_t) __a, (int8x16_t) __b, __c);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vsriq_n_u16 (uint16x8_t __a, uint16x8_t __b, const int __c)
{
  return (uint16x8_t)__builtin_neon_vsri_nv8hi ((int16x8_t) __a, (int16x8_t) __b, __c);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vsriq_n_u32 (uint32x4_t __a, uint32x4_t __b, const int __c)
{
  return (uint32x4_t)__builtin_neon_vsri_nv4si ((int32x4_t) __a, (int32x4_t) __b, __c);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vsriq_n_u64 (uint64x2_t __a, uint64x2_t __b, const int __c)
{
  return (uint64x2_t)__builtin_neon_vsri_nv2di ((int64x2_t) __a, (int64x2_t) __b, __c);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vsriq_n_p8 (poly8x16_t __a, poly8x16_t __b, const int __c)
{
  return (poly8x16_t)__builtin_neon_vsri_nv16qi ((int8x16_t) __a, (int8x16_t) __b, __c);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vsriq_n_p16 (poly16x8_t __a, poly16x8_t __b, const int __c)
{
  return (poly16x8_t)__builtin_neon_vsri_nv8hi ((int16x8_t) __a, (int16x8_t) __b, __c);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vsli_n_s8 (int8x8_t __a, int8x8_t __b, const int __c)
{
  return (int8x8_t)__builtin_neon_vsli_nv8qi (__a, __b, __c);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vsli_n_s16 (int16x4_t __a, int16x4_t __b, const int __c)
{
  return (int16x4_t)__builtin_neon_vsli_nv4hi (__a, __b, __c);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vsli_n_s32 (int32x2_t __a, int32x2_t __b, const int __c)
{
  return (int32x2_t)__builtin_neon_vsli_nv2si (__a, __b, __c);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vsli_n_s64 (int64x1_t __a, int64x1_t __b, const int __c)
{
  return (int64x1_t)__builtin_neon_vsli_ndi (__a, __b, __c);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vsli_n_u8 (uint8x8_t __a, uint8x8_t __b, const int __c)
{
  return (uint8x8_t)__builtin_neon_vsli_nv8qi ((int8x8_t) __a, (int8x8_t) __b, __c);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vsli_n_u16 (uint16x4_t __a, uint16x4_t __b, const int __c)
{
  return (uint16x4_t)__builtin_neon_vsli_nv4hi ((int16x4_t) __a, (int16x4_t) __b, __c);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vsli_n_u32 (uint32x2_t __a, uint32x2_t __b, const int __c)
{
  return (uint32x2_t)__builtin_neon_vsli_nv2si ((int32x2_t) __a, (int32x2_t) __b, __c);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vsli_n_u64 (uint64x1_t __a, uint64x1_t __b, const int __c)
{
  return (uint64x1_t)__builtin_neon_vsli_ndi ((int64x1_t) __a, (int64x1_t) __b, __c);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vsli_n_p8 (poly8x8_t __a, poly8x8_t __b, const int __c)
{
  return (poly8x8_t)__builtin_neon_vsli_nv8qi ((int8x8_t) __a, (int8x8_t) __b, __c);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vsli_n_p16 (poly16x4_t __a, poly16x4_t __b, const int __c)
{
  return (poly16x4_t)__builtin_neon_vsli_nv4hi ((int16x4_t) __a, (int16x4_t) __b, __c);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vsliq_n_s8 (int8x16_t __a, int8x16_t __b, const int __c)
{
  return (int8x16_t)__builtin_neon_vsli_nv16qi (__a, __b, __c);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vsliq_n_s16 (int16x8_t __a, int16x8_t __b, const int __c)
{
  return (int16x8_t)__builtin_neon_vsli_nv8hi (__a, __b, __c);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vsliq_n_s32 (int32x4_t __a, int32x4_t __b, const int __c)
{
  return (int32x4_t)__builtin_neon_vsli_nv4si (__a, __b, __c);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vsliq_n_s64 (int64x2_t __a, int64x2_t __b, const int __c)
{
  return (int64x2_t)__builtin_neon_vsli_nv2di (__a, __b, __c);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vsliq_n_u8 (uint8x16_t __a, uint8x16_t __b, const int __c)
{
  return (uint8x16_t)__builtin_neon_vsli_nv16qi ((int8x16_t) __a, (int8x16_t) __b, __c);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vsliq_n_u16 (uint16x8_t __a, uint16x8_t __b, const int __c)
{
  return (uint16x8_t)__builtin_neon_vsli_nv8hi ((int16x8_t) __a, (int16x8_t) __b, __c);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vsliq_n_u32 (uint32x4_t __a, uint32x4_t __b, const int __c)
{
  return (uint32x4_t)__builtin_neon_vsli_nv4si ((int32x4_t) __a, (int32x4_t) __b, __c);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vsliq_n_u64 (uint64x2_t __a, uint64x2_t __b, const int __c)
{
  return (uint64x2_t)__builtin_neon_vsli_nv2di ((int64x2_t) __a, (int64x2_t) __b, __c);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vsliq_n_p8 (poly8x16_t __a, poly8x16_t __b, const int __c)
{
  return (poly8x16_t)__builtin_neon_vsli_nv16qi ((int8x16_t) __a, (int8x16_t) __b, __c);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vsliq_n_p16 (poly16x8_t __a, poly16x8_t __b, const int __c)
{
  return (poly16x8_t)__builtin_neon_vsli_nv8hi ((int16x8_t) __a, (int16x8_t) __b, __c);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vabs_s8 (int8x8_t __a)
{
  return (int8x8_t)__builtin_neon_vabsv8qi (__a, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vabs_s16 (int16x4_t __a)
{
  return (int16x4_t)__builtin_neon_vabsv4hi (__a, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vabs_s32 (int32x2_t __a)
{
  return (int32x2_t)__builtin_neon_vabsv2si (__a, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vabs_f32 (float32x2_t __a)
{
  return (float32x2_t)__builtin_neon_vabsv2sf (__a, 3);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vabsq_s8 (int8x16_t __a)
{
  return (int8x16_t)__builtin_neon_vabsv16qi (__a, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vabsq_s16 (int16x8_t __a)
{
  return (int16x8_t)__builtin_neon_vabsv8hi (__a, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vabsq_s32 (int32x4_t __a)
{
  return (int32x4_t)__builtin_neon_vabsv4si (__a, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vabsq_f32 (float32x4_t __a)
{
  return (float32x4_t)__builtin_neon_vabsv4sf (__a, 3);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vqabs_s8 (int8x8_t __a)
{
  return (int8x8_t)__builtin_neon_vqabsv8qi (__a, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vqabs_s16 (int16x4_t __a)
{
  return (int16x4_t)__builtin_neon_vqabsv4hi (__a, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vqabs_s32 (int32x2_t __a)
{
  return (int32x2_t)__builtin_neon_vqabsv2si (__a, 1);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vqabsq_s8 (int8x16_t __a)
{
  return (int8x16_t)__builtin_neon_vqabsv16qi (__a, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vqabsq_s16 (int16x8_t __a)
{
  return (int16x8_t)__builtin_neon_vqabsv8hi (__a, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqabsq_s32 (int32x4_t __a)
{
  return (int32x4_t)__builtin_neon_vqabsv4si (__a, 1);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vneg_s8 (int8x8_t __a)
{
  return (int8x8_t)__builtin_neon_vnegv8qi (__a, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vneg_s16 (int16x4_t __a)
{
  return (int16x4_t)__builtin_neon_vnegv4hi (__a, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vneg_s32 (int32x2_t __a)
{
  return (int32x2_t)__builtin_neon_vnegv2si (__a, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vneg_f32 (float32x2_t __a)
{
  return (float32x2_t)__builtin_neon_vnegv2sf (__a, 3);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vnegq_s8 (int8x16_t __a)
{
  return (int8x16_t)__builtin_neon_vnegv16qi (__a, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vnegq_s16 (int16x8_t __a)
{
  return (int16x8_t)__builtin_neon_vnegv8hi (__a, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vnegq_s32 (int32x4_t __a)
{
  return (int32x4_t)__builtin_neon_vnegv4si (__a, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vnegq_f32 (float32x4_t __a)
{
  return (float32x4_t)__builtin_neon_vnegv4sf (__a, 3);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vqneg_s8 (int8x8_t __a)
{
  return (int8x8_t)__builtin_neon_vqnegv8qi (__a, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vqneg_s16 (int16x4_t __a)
{
  return (int16x4_t)__builtin_neon_vqnegv4hi (__a, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vqneg_s32 (int32x2_t __a)
{
  return (int32x2_t)__builtin_neon_vqnegv2si (__a, 1);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vqnegq_s8 (int8x16_t __a)
{
  return (int8x16_t)__builtin_neon_vqnegv16qi (__a, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vqnegq_s16 (int16x8_t __a)
{
  return (int16x8_t)__builtin_neon_vqnegv8hi (__a, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqnegq_s32 (int32x4_t __a)
{
  return (int32x4_t)__builtin_neon_vqnegv4si (__a, 1);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vmvn_s8 (int8x8_t __a)
{
  return (int8x8_t)__builtin_neon_vmvnv8qi (__a, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vmvn_s16 (int16x4_t __a)
{
  return (int16x4_t)__builtin_neon_vmvnv4hi (__a, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vmvn_s32 (int32x2_t __a)
{
  return (int32x2_t)__builtin_neon_vmvnv2si (__a, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vmvn_u8 (uint8x8_t __a)
{
  return (uint8x8_t)__builtin_neon_vmvnv8qi ((int8x8_t) __a, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vmvn_u16 (uint16x4_t __a)
{
  return (uint16x4_t)__builtin_neon_vmvnv4hi ((int16x4_t) __a, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vmvn_u32 (uint32x2_t __a)
{
  return (uint32x2_t)__builtin_neon_vmvnv2si ((int32x2_t) __a, 0);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vmvn_p8 (poly8x8_t __a)
{
  return (poly8x8_t)__builtin_neon_vmvnv8qi ((int8x8_t) __a, 2);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vmvnq_s8 (int8x16_t __a)
{
  return (int8x16_t)__builtin_neon_vmvnv16qi (__a, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vmvnq_s16 (int16x8_t __a)
{
  return (int16x8_t)__builtin_neon_vmvnv8hi (__a, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmvnq_s32 (int32x4_t __a)
{
  return (int32x4_t)__builtin_neon_vmvnv4si (__a, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vmvnq_u8 (uint8x16_t __a)
{
  return (uint8x16_t)__builtin_neon_vmvnv16qi ((int8x16_t) __a, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vmvnq_u16 (uint16x8_t __a)
{
  return (uint16x8_t)__builtin_neon_vmvnv8hi ((int16x8_t) __a, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmvnq_u32 (uint32x4_t __a)
{
  return (uint32x4_t)__builtin_neon_vmvnv4si ((int32x4_t) __a, 0);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vmvnq_p8 (poly8x16_t __a)
{
  return (poly8x16_t)__builtin_neon_vmvnv16qi ((int8x16_t) __a, 2);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vcls_s8 (int8x8_t __a)
{
  return (int8x8_t)__builtin_neon_vclsv8qi (__a, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vcls_s16 (int16x4_t __a)
{
  return (int16x4_t)__builtin_neon_vclsv4hi (__a, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vcls_s32 (int32x2_t __a)
{
  return (int32x2_t)__builtin_neon_vclsv2si (__a, 1);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vclsq_s8 (int8x16_t __a)
{
  return (int8x16_t)__builtin_neon_vclsv16qi (__a, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vclsq_s16 (int16x8_t __a)
{
  return (int16x8_t)__builtin_neon_vclsv8hi (__a, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vclsq_s32 (int32x4_t __a)
{
  return (int32x4_t)__builtin_neon_vclsv4si (__a, 1);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vclz_s8 (int8x8_t __a)
{
  return (int8x8_t)__builtin_neon_vclzv8qi (__a, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vclz_s16 (int16x4_t __a)
{
  return (int16x4_t)__builtin_neon_vclzv4hi (__a, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vclz_s32 (int32x2_t __a)
{
  return (int32x2_t)__builtin_neon_vclzv2si (__a, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vclz_u8 (uint8x8_t __a)
{
  return (uint8x8_t)__builtin_neon_vclzv8qi ((int8x8_t) __a, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vclz_u16 (uint16x4_t __a)
{
  return (uint16x4_t)__builtin_neon_vclzv4hi ((int16x4_t) __a, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vclz_u32 (uint32x2_t __a)
{
  return (uint32x2_t)__builtin_neon_vclzv2si ((int32x2_t) __a, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vclzq_s8 (int8x16_t __a)
{
  return (int8x16_t)__builtin_neon_vclzv16qi (__a, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vclzq_s16 (int16x8_t __a)
{
  return (int16x8_t)__builtin_neon_vclzv8hi (__a, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vclzq_s32 (int32x4_t __a)
{
  return (int32x4_t)__builtin_neon_vclzv4si (__a, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vclzq_u8 (uint8x16_t __a)
{
  return (uint8x16_t)__builtin_neon_vclzv16qi ((int8x16_t) __a, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vclzq_u16 (uint16x8_t __a)
{
  return (uint16x8_t)__builtin_neon_vclzv8hi ((int16x8_t) __a, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vclzq_u32 (uint32x4_t __a)
{
  return (uint32x4_t)__builtin_neon_vclzv4si ((int32x4_t) __a, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vcnt_s8 (int8x8_t __a)
{
  return (int8x8_t)__builtin_neon_vcntv8qi (__a, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vcnt_u8 (uint8x8_t __a)
{
  return (uint8x8_t)__builtin_neon_vcntv8qi ((int8x8_t) __a, 0);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vcnt_p8 (poly8x8_t __a)
{
  return (poly8x8_t)__builtin_neon_vcntv8qi ((int8x8_t) __a, 2);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vcntq_s8 (int8x16_t __a)
{
  return (int8x16_t)__builtin_neon_vcntv16qi (__a, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vcntq_u8 (uint8x16_t __a)
{
  return (uint8x16_t)__builtin_neon_vcntv16qi ((int8x16_t) __a, 0);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vcntq_p8 (poly8x16_t __a)
{
  return (poly8x16_t)__builtin_neon_vcntv16qi ((int8x16_t) __a, 2);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vrecpe_f32 (float32x2_t __a)
{
  return (float32x2_t)__builtin_neon_vrecpev2sf (__a, 3);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vrecpe_u32 (uint32x2_t __a)
{
  return (uint32x2_t)__builtin_neon_vrecpev2si ((int32x2_t) __a, 0);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vrecpeq_f32 (float32x4_t __a)
{
  return (float32x4_t)__builtin_neon_vrecpev4sf (__a, 3);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vrecpeq_u32 (uint32x4_t __a)
{
  return (uint32x4_t)__builtin_neon_vrecpev4si ((int32x4_t) __a, 0);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vrsqrte_f32 (float32x2_t __a)
{
  return (float32x2_t)__builtin_neon_vrsqrtev2sf (__a, 3);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vrsqrte_u32 (uint32x2_t __a)
{
  return (uint32x2_t)__builtin_neon_vrsqrtev2si ((int32x2_t) __a, 0);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vrsqrteq_f32 (float32x4_t __a)
{
  return (float32x4_t)__builtin_neon_vrsqrtev4sf (__a, 3);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vrsqrteq_u32 (uint32x4_t __a)
{
  return (uint32x4_t)__builtin_neon_vrsqrtev4si ((int32x4_t) __a, 0);
}

__extension__ static __inline int8_t __attribute__ ((__always_inline__))
vget_lane_s8 (int8x8_t __a, const int __b)
{
  return (int8_t)__builtin_neon_vget_lanev8qi (__a, __b, 1);
}

__extension__ static __inline int16_t __attribute__ ((__always_inline__))
vget_lane_s16 (int16x4_t __a, const int __b)
{
  return (int16_t)__builtin_neon_vget_lanev4hi (__a, __b, 1);
}

__extension__ static __inline int32_t __attribute__ ((__always_inline__))
vget_lane_s32 (int32x2_t __a, const int __b)
{
  return (int32_t)__builtin_neon_vget_lanev2si (__a, __b, 1);
}

__extension__ static __inline float32_t __attribute__ ((__always_inline__))
vget_lane_f32 (float32x2_t __a, const int __b)
{
  return (float32_t)__builtin_neon_vget_lanev2sf (__a, __b, 3);
}

__extension__ static __inline uint8_t __attribute__ ((__always_inline__))
vget_lane_u8 (uint8x8_t __a, const int __b)
{
  return (uint8_t)__builtin_neon_vget_lanev8qi ((int8x8_t) __a, __b, 0);
}

__extension__ static __inline uint16_t __attribute__ ((__always_inline__))
vget_lane_u16 (uint16x4_t __a, const int __b)
{
  return (uint16_t)__builtin_neon_vget_lanev4hi ((int16x4_t) __a, __b, 0);
}

__extension__ static __inline uint32_t __attribute__ ((__always_inline__))
vget_lane_u32 (uint32x2_t __a, const int __b)
{
  return (uint32_t)__builtin_neon_vget_lanev2si ((int32x2_t) __a, __b, 0);
}

__extension__ static __inline poly8_t __attribute__ ((__always_inline__))
vget_lane_p8 (poly8x8_t __a, const int __b)
{
  return (poly8_t)__builtin_neon_vget_lanev8qi ((int8x8_t) __a, __b, 2);
}

__extension__ static __inline poly16_t __attribute__ ((__always_inline__))
vget_lane_p16 (poly16x4_t __a, const int __b)
{
  return (poly16_t)__builtin_neon_vget_lanev4hi ((int16x4_t) __a, __b, 2);
}

__extension__ static __inline int64_t __attribute__ ((__always_inline__))
vget_lane_s64 (int64x1_t __a, const int __b)
{
  return (int64_t)__builtin_neon_vget_lanedi (__a, __b, 1);
}

__extension__ static __inline uint64_t __attribute__ ((__always_inline__))
vget_lane_u64 (uint64x1_t __a, const int __b)
{
  return (uint64_t)__builtin_neon_vget_lanedi ((int64x1_t) __a, __b, 0);
}

__extension__ static __inline int8_t __attribute__ ((__always_inline__))
vgetq_lane_s8 (int8x16_t __a, const int __b)
{
  return (int8_t)__builtin_neon_vget_lanev16qi (__a, __b, 1);
}

__extension__ static __inline int16_t __attribute__ ((__always_inline__))
vgetq_lane_s16 (int16x8_t __a, const int __b)
{
  return (int16_t)__builtin_neon_vget_lanev8hi (__a, __b, 1);
}

__extension__ static __inline int32_t __attribute__ ((__always_inline__))
vgetq_lane_s32 (int32x4_t __a, const int __b)
{
  return (int32_t)__builtin_neon_vget_lanev4si (__a, __b, 1);
}

__extension__ static __inline float32_t __attribute__ ((__always_inline__))
vgetq_lane_f32 (float32x4_t __a, const int __b)
{
  return (float32_t)__builtin_neon_vget_lanev4sf (__a, __b, 3);
}

__extension__ static __inline uint8_t __attribute__ ((__always_inline__))
vgetq_lane_u8 (uint8x16_t __a, const int __b)
{
  return (uint8_t)__builtin_neon_vget_lanev16qi ((int8x16_t) __a, __b, 0);
}

__extension__ static __inline uint16_t __attribute__ ((__always_inline__))
vgetq_lane_u16 (uint16x8_t __a, const int __b)
{
  return (uint16_t)__builtin_neon_vget_lanev8hi ((int16x8_t) __a, __b, 0);
}

__extension__ static __inline uint32_t __attribute__ ((__always_inline__))
vgetq_lane_u32 (uint32x4_t __a, const int __b)
{
  return (uint32_t)__builtin_neon_vget_lanev4si ((int32x4_t) __a, __b, 0);
}

__extension__ static __inline poly8_t __attribute__ ((__always_inline__))
vgetq_lane_p8 (poly8x16_t __a, const int __b)
{
  return (poly8_t)__builtin_neon_vget_lanev16qi ((int8x16_t) __a, __b, 2);
}

__extension__ static __inline poly16_t __attribute__ ((__always_inline__))
vgetq_lane_p16 (poly16x8_t __a, const int __b)
{
  return (poly16_t)__builtin_neon_vget_lanev8hi ((int16x8_t) __a, __b, 2);
}

__extension__ static __inline int64_t __attribute__ ((__always_inline__))
vgetq_lane_s64 (int64x2_t __a, const int __b)
{
  return (int64_t)__builtin_neon_vget_lanev2di (__a, __b, 1);
}

__extension__ static __inline uint64_t __attribute__ ((__always_inline__))
vgetq_lane_u64 (uint64x2_t __a, const int __b)
{
  return (uint64_t)__builtin_neon_vget_lanev2di ((int64x2_t) __a, __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vset_lane_s8 (int8_t __a, int8x8_t __b, const int __c)
{
  return (int8x8_t)__builtin_neon_vset_lanev8qi ((__builtin_neon_qi) __a, __b, __c);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vset_lane_s16 (int16_t __a, int16x4_t __b, const int __c)
{
  return (int16x4_t)__builtin_neon_vset_lanev4hi ((__builtin_neon_hi) __a, __b, __c);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vset_lane_s32 (int32_t __a, int32x2_t __b, const int __c)
{
  return (int32x2_t)__builtin_neon_vset_lanev2si ((__builtin_neon_si) __a, __b, __c);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vset_lane_f32 (float32_t __a, float32x2_t __b, const int __c)
{
  return (float32x2_t)__builtin_neon_vset_lanev2sf ((__builtin_neon_sf) __a, __b, __c);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vset_lane_u8 (uint8_t __a, uint8x8_t __b, const int __c)
{
  return (uint8x8_t)__builtin_neon_vset_lanev8qi ((__builtin_neon_qi) __a, (int8x8_t) __b, __c);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vset_lane_u16 (uint16_t __a, uint16x4_t __b, const int __c)
{
  return (uint16x4_t)__builtin_neon_vset_lanev4hi ((__builtin_neon_hi) __a, (int16x4_t) __b, __c);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vset_lane_u32 (uint32_t __a, uint32x2_t __b, const int __c)
{
  return (uint32x2_t)__builtin_neon_vset_lanev2si ((__builtin_neon_si) __a, (int32x2_t) __b, __c);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vset_lane_p8 (poly8_t __a, poly8x8_t __b, const int __c)
{
  return (poly8x8_t)__builtin_neon_vset_lanev8qi ((__builtin_neon_qi) __a, (int8x8_t) __b, __c);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vset_lane_p16 (poly16_t __a, poly16x4_t __b, const int __c)
{
  return (poly16x4_t)__builtin_neon_vset_lanev4hi ((__builtin_neon_hi) __a, (int16x4_t) __b, __c);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vset_lane_s64 (int64_t __a, int64x1_t __b, const int __c)
{
  return (int64x1_t)__builtin_neon_vset_lanedi ((__builtin_neon_di) __a, __b, __c);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vset_lane_u64 (uint64_t __a, uint64x1_t __b, const int __c)
{
  return (uint64x1_t)__builtin_neon_vset_lanedi ((__builtin_neon_di) __a, (int64x1_t) __b, __c);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vsetq_lane_s8 (int8_t __a, int8x16_t __b, const int __c)
{
  return (int8x16_t)__builtin_neon_vset_lanev16qi ((__builtin_neon_qi) __a, __b, __c);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vsetq_lane_s16 (int16_t __a, int16x8_t __b, const int __c)
{
  return (int16x8_t)__builtin_neon_vset_lanev8hi ((__builtin_neon_hi) __a, __b, __c);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vsetq_lane_s32 (int32_t __a, int32x4_t __b, const int __c)
{
  return (int32x4_t)__builtin_neon_vset_lanev4si ((__builtin_neon_si) __a, __b, __c);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vsetq_lane_f32 (float32_t __a, float32x4_t __b, const int __c)
{
  return (float32x4_t)__builtin_neon_vset_lanev4sf ((__builtin_neon_sf) __a, __b, __c);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vsetq_lane_u8 (uint8_t __a, uint8x16_t __b, const int __c)
{
  return (uint8x16_t)__builtin_neon_vset_lanev16qi ((__builtin_neon_qi) __a, (int8x16_t) __b, __c);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vsetq_lane_u16 (uint16_t __a, uint16x8_t __b, const int __c)
{
  return (uint16x8_t)__builtin_neon_vset_lanev8hi ((__builtin_neon_hi) __a, (int16x8_t) __b, __c);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vsetq_lane_u32 (uint32_t __a, uint32x4_t __b, const int __c)
{
  return (uint32x4_t)__builtin_neon_vset_lanev4si ((__builtin_neon_si) __a, (int32x4_t) __b, __c);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vsetq_lane_p8 (poly8_t __a, poly8x16_t __b, const int __c)
{
  return (poly8x16_t)__builtin_neon_vset_lanev16qi ((__builtin_neon_qi) __a, (int8x16_t) __b, __c);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vsetq_lane_p16 (poly16_t __a, poly16x8_t __b, const int __c)
{
  return (poly16x8_t)__builtin_neon_vset_lanev8hi ((__builtin_neon_hi) __a, (int16x8_t) __b, __c);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vsetq_lane_s64 (int64_t __a, int64x2_t __b, const int __c)
{
  return (int64x2_t)__builtin_neon_vset_lanev2di ((__builtin_neon_di) __a, __b, __c);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vsetq_lane_u64 (uint64_t __a, uint64x2_t __b, const int __c)
{
  return (uint64x2_t)__builtin_neon_vset_lanev2di ((__builtin_neon_di) __a, (int64x2_t) __b, __c);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vcreate_s8 (uint64_t __a)
{
  return (int8x8_t)__builtin_neon_vcreatev8qi ((__builtin_neon_di) __a);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vcreate_s16 (uint64_t __a)
{
  return (int16x4_t)__builtin_neon_vcreatev4hi ((__builtin_neon_di) __a);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vcreate_s32 (uint64_t __a)
{
  return (int32x2_t)__builtin_neon_vcreatev2si ((__builtin_neon_di) __a);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vcreate_s64 (uint64_t __a)
{
  return (int64x1_t)__builtin_neon_vcreatedi ((__builtin_neon_di) __a);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vcreate_f32 (uint64_t __a)
{
  return (float32x2_t)__builtin_neon_vcreatev2sf ((__builtin_neon_di) __a);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vcreate_u8 (uint64_t __a)
{
  return (uint8x8_t)__builtin_neon_vcreatev8qi ((__builtin_neon_di) __a);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vcreate_u16 (uint64_t __a)
{
  return (uint16x4_t)__builtin_neon_vcreatev4hi ((__builtin_neon_di) __a);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vcreate_u32 (uint64_t __a)
{
  return (uint32x2_t)__builtin_neon_vcreatev2si ((__builtin_neon_di) __a);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vcreate_u64 (uint64_t __a)
{
  return (uint64x1_t)__builtin_neon_vcreatedi ((__builtin_neon_di) __a);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vcreate_p8 (uint64_t __a)
{
  return (poly8x8_t)__builtin_neon_vcreatev8qi ((__builtin_neon_di) __a);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vcreate_p16 (uint64_t __a)
{
  return (poly16x4_t)__builtin_neon_vcreatev4hi ((__builtin_neon_di) __a);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vdup_n_s8 (int8_t __a)
{
  return (int8x8_t)__builtin_neon_vdup_nv8qi ((__builtin_neon_qi) __a);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vdup_n_s16 (int16_t __a)
{
  return (int16x4_t)__builtin_neon_vdup_nv4hi ((__builtin_neon_hi) __a);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vdup_n_s32 (int32_t __a)
{
  return (int32x2_t)__builtin_neon_vdup_nv2si ((__builtin_neon_si) __a);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vdup_n_f32 (float32_t __a)
{
  return (float32x2_t)__builtin_neon_vdup_nv2sf ((__builtin_neon_sf) __a);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vdup_n_u8 (uint8_t __a)
{
  return (uint8x8_t)__builtin_neon_vdup_nv8qi ((__builtin_neon_qi) __a);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vdup_n_u16 (uint16_t __a)
{
  return (uint16x4_t)__builtin_neon_vdup_nv4hi ((__builtin_neon_hi) __a);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vdup_n_u32 (uint32_t __a)
{
  return (uint32x2_t)__builtin_neon_vdup_nv2si ((__builtin_neon_si) __a);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vdup_n_p8 (poly8_t __a)
{
  return (poly8x8_t)__builtin_neon_vdup_nv8qi ((__builtin_neon_qi) __a);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vdup_n_p16 (poly16_t __a)
{
  return (poly16x4_t)__builtin_neon_vdup_nv4hi ((__builtin_neon_hi) __a);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vdup_n_s64 (int64_t __a)
{
  return (int64x1_t)__builtin_neon_vdup_ndi ((__builtin_neon_di) __a);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vdup_n_u64 (uint64_t __a)
{
  return (uint64x1_t)__builtin_neon_vdup_ndi ((__builtin_neon_di) __a);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vdupq_n_s8 (int8_t __a)
{
  return (int8x16_t)__builtin_neon_vdup_nv16qi ((__builtin_neon_qi) __a);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vdupq_n_s16 (int16_t __a)
{
  return (int16x8_t)__builtin_neon_vdup_nv8hi ((__builtin_neon_hi) __a);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vdupq_n_s32 (int32_t __a)
{
  return (int32x4_t)__builtin_neon_vdup_nv4si ((__builtin_neon_si) __a);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vdupq_n_f32 (float32_t __a)
{
  return (float32x4_t)__builtin_neon_vdup_nv4sf ((__builtin_neon_sf) __a);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vdupq_n_u8 (uint8_t __a)
{
  return (uint8x16_t)__builtin_neon_vdup_nv16qi ((__builtin_neon_qi) __a);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vdupq_n_u16 (uint16_t __a)
{
  return (uint16x8_t)__builtin_neon_vdup_nv8hi ((__builtin_neon_hi) __a);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vdupq_n_u32 (uint32_t __a)
{
  return (uint32x4_t)__builtin_neon_vdup_nv4si ((__builtin_neon_si) __a);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vdupq_n_p8 (poly8_t __a)
{
  return (poly8x16_t)__builtin_neon_vdup_nv16qi ((__builtin_neon_qi) __a);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vdupq_n_p16 (poly16_t __a)
{
  return (poly16x8_t)__builtin_neon_vdup_nv8hi ((__builtin_neon_hi) __a);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vdupq_n_s64 (int64_t __a)
{
  return (int64x2_t)__builtin_neon_vdup_nv2di ((__builtin_neon_di) __a);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vdupq_n_u64 (uint64_t __a)
{
  return (uint64x2_t)__builtin_neon_vdup_nv2di ((__builtin_neon_di) __a);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vmov_n_s8 (int8_t __a)
{
  return (int8x8_t)__builtin_neon_vdup_nv8qi ((__builtin_neon_qi) __a);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vmov_n_s16 (int16_t __a)
{
  return (int16x4_t)__builtin_neon_vdup_nv4hi ((__builtin_neon_hi) __a);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vmov_n_s32 (int32_t __a)
{
  return (int32x2_t)__builtin_neon_vdup_nv2si ((__builtin_neon_si) __a);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vmov_n_f32 (float32_t __a)
{
  return (float32x2_t)__builtin_neon_vdup_nv2sf ((__builtin_neon_sf) __a);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vmov_n_u8 (uint8_t __a)
{
  return (uint8x8_t)__builtin_neon_vdup_nv8qi ((__builtin_neon_qi) __a);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vmov_n_u16 (uint16_t __a)
{
  return (uint16x4_t)__builtin_neon_vdup_nv4hi ((__builtin_neon_hi) __a);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vmov_n_u32 (uint32_t __a)
{
  return (uint32x2_t)__builtin_neon_vdup_nv2si ((__builtin_neon_si) __a);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vmov_n_p8 (poly8_t __a)
{
  return (poly8x8_t)__builtin_neon_vdup_nv8qi ((__builtin_neon_qi) __a);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vmov_n_p16 (poly16_t __a)
{
  return (poly16x4_t)__builtin_neon_vdup_nv4hi ((__builtin_neon_hi) __a);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vmov_n_s64 (int64_t __a)
{
  return (int64x1_t)__builtin_neon_vdup_ndi ((__builtin_neon_di) __a);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vmov_n_u64 (uint64_t __a)
{
  return (uint64x1_t)__builtin_neon_vdup_ndi ((__builtin_neon_di) __a);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vmovq_n_s8 (int8_t __a)
{
  return (int8x16_t)__builtin_neon_vdup_nv16qi ((__builtin_neon_qi) __a);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vmovq_n_s16 (int16_t __a)
{
  return (int16x8_t)__builtin_neon_vdup_nv8hi ((__builtin_neon_hi) __a);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmovq_n_s32 (int32_t __a)
{
  return (int32x4_t)__builtin_neon_vdup_nv4si ((__builtin_neon_si) __a);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vmovq_n_f32 (float32_t __a)
{
  return (float32x4_t)__builtin_neon_vdup_nv4sf ((__builtin_neon_sf) __a);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vmovq_n_u8 (uint8_t __a)
{
  return (uint8x16_t)__builtin_neon_vdup_nv16qi ((__builtin_neon_qi) __a);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vmovq_n_u16 (uint16_t __a)
{
  return (uint16x8_t)__builtin_neon_vdup_nv8hi ((__builtin_neon_hi) __a);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmovq_n_u32 (uint32_t __a)
{
  return (uint32x4_t)__builtin_neon_vdup_nv4si ((__builtin_neon_si) __a);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vmovq_n_p8 (poly8_t __a)
{
  return (poly8x16_t)__builtin_neon_vdup_nv16qi ((__builtin_neon_qi) __a);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vmovq_n_p16 (poly16_t __a)
{
  return (poly16x8_t)__builtin_neon_vdup_nv8hi ((__builtin_neon_hi) __a);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vmovq_n_s64 (int64_t __a)
{
  return (int64x2_t)__builtin_neon_vdup_nv2di ((__builtin_neon_di) __a);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vmovq_n_u64 (uint64_t __a)
{
  return (uint64x2_t)__builtin_neon_vdup_nv2di ((__builtin_neon_di) __a);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vdup_lane_s8 (int8x8_t __a, const int __b)
{
  return (int8x8_t)__builtin_neon_vdup_lanev8qi (__a, __b);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vdup_lane_s16 (int16x4_t __a, const int __b)
{
  return (int16x4_t)__builtin_neon_vdup_lanev4hi (__a, __b);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vdup_lane_s32 (int32x2_t __a, const int __b)
{
  return (int32x2_t)__builtin_neon_vdup_lanev2si (__a, __b);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vdup_lane_f32 (float32x2_t __a, const int __b)
{
  return (float32x2_t)__builtin_neon_vdup_lanev2sf (__a, __b);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vdup_lane_u8 (uint8x8_t __a, const int __b)
{
  return (uint8x8_t)__builtin_neon_vdup_lanev8qi ((int8x8_t) __a, __b);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vdup_lane_u16 (uint16x4_t __a, const int __b)
{
  return (uint16x4_t)__builtin_neon_vdup_lanev4hi ((int16x4_t) __a, __b);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vdup_lane_u32 (uint32x2_t __a, const int __b)
{
  return (uint32x2_t)__builtin_neon_vdup_lanev2si ((int32x2_t) __a, __b);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vdup_lane_p8 (poly8x8_t __a, const int __b)
{
  return (poly8x8_t)__builtin_neon_vdup_lanev8qi ((int8x8_t) __a, __b);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vdup_lane_p16 (poly16x4_t __a, const int __b)
{
  return (poly16x4_t)__builtin_neon_vdup_lanev4hi ((int16x4_t) __a, __b);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vdup_lane_s64 (int64x1_t __a, const int __b)
{
  return (int64x1_t)__builtin_neon_vdup_lanedi (__a, __b);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vdup_lane_u64 (uint64x1_t __a, const int __b)
{
  return (uint64x1_t)__builtin_neon_vdup_lanedi ((int64x1_t) __a, __b);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vdupq_lane_s8 (int8x8_t __a, const int __b)
{
  return (int8x16_t)__builtin_neon_vdup_lanev16qi (__a, __b);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vdupq_lane_s16 (int16x4_t __a, const int __b)
{
  return (int16x8_t)__builtin_neon_vdup_lanev8hi (__a, __b);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vdupq_lane_s32 (int32x2_t __a, const int __b)
{
  return (int32x4_t)__builtin_neon_vdup_lanev4si (__a, __b);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vdupq_lane_f32 (float32x2_t __a, const int __b)
{
  return (float32x4_t)__builtin_neon_vdup_lanev4sf (__a, __b);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vdupq_lane_u8 (uint8x8_t __a, const int __b)
{
  return (uint8x16_t)__builtin_neon_vdup_lanev16qi ((int8x8_t) __a, __b);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vdupq_lane_u16 (uint16x4_t __a, const int __b)
{
  return (uint16x8_t)__builtin_neon_vdup_lanev8hi ((int16x4_t) __a, __b);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vdupq_lane_u32 (uint32x2_t __a, const int __b)
{
  return (uint32x4_t)__builtin_neon_vdup_lanev4si ((int32x2_t) __a, __b);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vdupq_lane_p8 (poly8x8_t __a, const int __b)
{
  return (poly8x16_t)__builtin_neon_vdup_lanev16qi ((int8x8_t) __a, __b);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vdupq_lane_p16 (poly16x4_t __a, const int __b)
{
  return (poly16x8_t)__builtin_neon_vdup_lanev8hi ((int16x4_t) __a, __b);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vdupq_lane_s64 (int64x1_t __a, const int __b)
{
  return (int64x2_t)__builtin_neon_vdup_lanev2di (__a, __b);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vdupq_lane_u64 (uint64x1_t __a, const int __b)
{
  return (uint64x2_t)__builtin_neon_vdup_lanev2di ((int64x1_t) __a, __b);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vcombine_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x16_t)__builtin_neon_vcombinev8qi (__a, __b);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vcombine_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x8_t)__builtin_neon_vcombinev4hi (__a, __b);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vcombine_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x4_t)__builtin_neon_vcombinev2si (__a, __b);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vcombine_s64 (int64x1_t __a, int64x1_t __b)
{
  return (int64x2_t)__builtin_neon_vcombinedi (__a, __b);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vcombine_f32 (float32x2_t __a, float32x2_t __b)
{
  return (float32x4_t)__builtin_neon_vcombinev2sf (__a, __b);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vcombine_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x16_t)__builtin_neon_vcombinev8qi ((int8x8_t) __a, (int8x8_t) __b);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vcombine_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x8_t)__builtin_neon_vcombinev4hi ((int16x4_t) __a, (int16x4_t) __b);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vcombine_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x4_t)__builtin_neon_vcombinev2si ((int32x2_t) __a, (int32x2_t) __b);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vcombine_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return (uint64x2_t)__builtin_neon_vcombinedi ((int64x1_t) __a, (int64x1_t) __b);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vcombine_p8 (poly8x8_t __a, poly8x8_t __b)
{
  return (poly8x16_t)__builtin_neon_vcombinev8qi ((int8x8_t) __a, (int8x8_t) __b);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vcombine_p16 (poly16x4_t __a, poly16x4_t __b)
{
  return (poly16x8_t)__builtin_neon_vcombinev4hi ((int16x4_t) __a, (int16x4_t) __b);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vget_high_s8 (int8x16_t __a)
{
  return (int8x8_t)__builtin_neon_vget_highv16qi (__a);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vget_high_s16 (int16x8_t __a)
{
  return (int16x4_t)__builtin_neon_vget_highv8hi (__a);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vget_high_s32 (int32x4_t __a)
{
  return (int32x2_t)__builtin_neon_vget_highv4si (__a);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vget_high_s64 (int64x2_t __a)
{
  return (int64x1_t)__builtin_neon_vget_highv2di (__a);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vget_high_f32 (float32x4_t __a)
{
  return (float32x2_t)__builtin_neon_vget_highv4sf (__a);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vget_high_u8 (uint8x16_t __a)
{
  return (uint8x8_t)__builtin_neon_vget_highv16qi ((int8x16_t) __a);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vget_high_u16 (uint16x8_t __a)
{
  return (uint16x4_t)__builtin_neon_vget_highv8hi ((int16x8_t) __a);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vget_high_u32 (uint32x4_t __a)
{
  return (uint32x2_t)__builtin_neon_vget_highv4si ((int32x4_t) __a);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vget_high_u64 (uint64x2_t __a)
{
  return (uint64x1_t)__builtin_neon_vget_highv2di ((int64x2_t) __a);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vget_high_p8 (poly8x16_t __a)
{
  return (poly8x8_t)__builtin_neon_vget_highv16qi ((int8x16_t) __a);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vget_high_p16 (poly16x8_t __a)
{
  return (poly16x4_t)__builtin_neon_vget_highv8hi ((int16x8_t) __a);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vget_low_s8 (int8x16_t __a)
{
  return (int8x8_t)__builtin_neon_vget_lowv16qi (__a);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vget_low_s16 (int16x8_t __a)
{
  return (int16x4_t)__builtin_neon_vget_lowv8hi (__a);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vget_low_s32 (int32x4_t __a)
{
  return (int32x2_t)__builtin_neon_vget_lowv4si (__a);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vget_low_f32 (float32x4_t __a)
{
  return (float32x2_t)__builtin_neon_vget_lowv4sf (__a);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vget_low_u8 (uint8x16_t __a)
{
  return (uint8x8_t)__builtin_neon_vget_lowv16qi ((int8x16_t) __a);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vget_low_u16 (uint16x8_t __a)
{
  return (uint16x4_t)__builtin_neon_vget_lowv8hi ((int16x8_t) __a);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vget_low_u32 (uint32x4_t __a)
{
  return (uint32x2_t)__builtin_neon_vget_lowv4si ((int32x4_t) __a);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vget_low_p8 (poly8x16_t __a)
{
  return (poly8x8_t)__builtin_neon_vget_lowv16qi ((int8x16_t) __a);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vget_low_p16 (poly16x8_t __a)
{
  return (poly16x4_t)__builtin_neon_vget_lowv8hi ((int16x8_t) __a);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vget_low_s64 (int64x2_t __a)
{
  return (int64x1_t)__builtin_neon_vget_lowv2di (__a);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vget_low_u64 (uint64x2_t __a)
{
  return (uint64x1_t)__builtin_neon_vget_lowv2di ((int64x2_t) __a);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vcvt_s32_f32 (float32x2_t __a)
{
  return (int32x2_t)__builtin_neon_vcvtv2sf (__a, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vcvt_f32_s32 (int32x2_t __a)
{
  return (float32x2_t)__builtin_neon_vcvtv2si (__a, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vcvt_f32_u32 (uint32x2_t __a)
{
  return (float32x2_t)__builtin_neon_vcvtv2si ((int32x2_t) __a, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vcvt_u32_f32 (float32x2_t __a)
{
  return (uint32x2_t)__builtin_neon_vcvtv2sf (__a, 0);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vcvtq_s32_f32 (float32x4_t __a)
{
  return (int32x4_t)__builtin_neon_vcvtv4sf (__a, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vcvtq_f32_s32 (int32x4_t __a)
{
  return (float32x4_t)__builtin_neon_vcvtv4si (__a, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vcvtq_f32_u32 (uint32x4_t __a)
{
  return (float32x4_t)__builtin_neon_vcvtv4si ((int32x4_t) __a, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vcvtq_u32_f32 (float32x4_t __a)
{
  return (uint32x4_t)__builtin_neon_vcvtv4sf (__a, 0);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vcvt_n_s32_f32 (float32x2_t __a, const int __b)
{
  return (int32x2_t)__builtin_neon_vcvt_nv2sf (__a, __b, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vcvt_n_f32_s32 (int32x2_t __a, const int __b)
{
  return (float32x2_t)__builtin_neon_vcvt_nv2si (__a, __b, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vcvt_n_f32_u32 (uint32x2_t __a, const int __b)
{
  return (float32x2_t)__builtin_neon_vcvt_nv2si ((int32x2_t) __a, __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vcvt_n_u32_f32 (float32x2_t __a, const int __b)
{
  return (uint32x2_t)__builtin_neon_vcvt_nv2sf (__a, __b, 0);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vcvtq_n_s32_f32 (float32x4_t __a, const int __b)
{
  return (int32x4_t)__builtin_neon_vcvt_nv4sf (__a, __b, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vcvtq_n_f32_s32 (int32x4_t __a, const int __b)
{
  return (float32x4_t)__builtin_neon_vcvt_nv4si (__a, __b, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vcvtq_n_f32_u32 (uint32x4_t __a, const int __b)
{
  return (float32x4_t)__builtin_neon_vcvt_nv4si ((int32x4_t) __a, __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vcvtq_n_u32_f32 (float32x4_t __a, const int __b)
{
  return (uint32x4_t)__builtin_neon_vcvt_nv4sf (__a, __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vmovn_s16 (int16x8_t __a)
{
  return (int8x8_t)__builtin_neon_vmovnv8hi (__a, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vmovn_s32 (int32x4_t __a)
{
  return (int16x4_t)__builtin_neon_vmovnv4si (__a, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vmovn_s64 (int64x2_t __a)
{
  return (int32x2_t)__builtin_neon_vmovnv2di (__a, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vmovn_u16 (uint16x8_t __a)
{
  return (uint8x8_t)__builtin_neon_vmovnv8hi ((int16x8_t) __a, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vmovn_u32 (uint32x4_t __a)
{
  return (uint16x4_t)__builtin_neon_vmovnv4si ((int32x4_t) __a, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vmovn_u64 (uint64x2_t __a)
{
  return (uint32x2_t)__builtin_neon_vmovnv2di ((int64x2_t) __a, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vqmovn_s16 (int16x8_t __a)
{
  return (int8x8_t)__builtin_neon_vqmovnv8hi (__a, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vqmovn_s32 (int32x4_t __a)
{
  return (int16x4_t)__builtin_neon_vqmovnv4si (__a, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vqmovn_s64 (int64x2_t __a)
{
  return (int32x2_t)__builtin_neon_vqmovnv2di (__a, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vqmovn_u16 (uint16x8_t __a)
{
  return (uint8x8_t)__builtin_neon_vqmovnv8hi ((int16x8_t) __a, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vqmovn_u32 (uint32x4_t __a)
{
  return (uint16x4_t)__builtin_neon_vqmovnv4si ((int32x4_t) __a, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vqmovn_u64 (uint64x2_t __a)
{
  return (uint32x2_t)__builtin_neon_vqmovnv2di ((int64x2_t) __a, 0);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vqmovun_s16 (int16x8_t __a)
{
  return (uint8x8_t)__builtin_neon_vqmovunv8hi (__a, 1);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vqmovun_s32 (int32x4_t __a)
{
  return (uint16x4_t)__builtin_neon_vqmovunv4si (__a, 1);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vqmovun_s64 (int64x2_t __a)
{
  return (uint32x2_t)__builtin_neon_vqmovunv2di (__a, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vmovl_s8 (int8x8_t __a)
{
  return (int16x8_t)__builtin_neon_vmovlv8qi (__a, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmovl_s16 (int16x4_t __a)
{
  return (int32x4_t)__builtin_neon_vmovlv4hi (__a, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vmovl_s32 (int32x2_t __a)
{
  return (int64x2_t)__builtin_neon_vmovlv2si (__a, 1);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vmovl_u8 (uint8x8_t __a)
{
  return (uint16x8_t)__builtin_neon_vmovlv8qi ((int8x8_t) __a, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmovl_u16 (uint16x4_t __a)
{
  return (uint32x4_t)__builtin_neon_vmovlv4hi ((int16x4_t) __a, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vmovl_u32 (uint32x2_t __a)
{
  return (uint64x2_t)__builtin_neon_vmovlv2si ((int32x2_t) __a, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vtbl1_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vtbl1v8qi (__a, __b);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vtbl1_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vtbl1v8qi ((int8x8_t) __a, (int8x8_t) __b);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vtbl1_p8 (poly8x8_t __a, uint8x8_t __b)
{
  return (poly8x8_t)__builtin_neon_vtbl1v8qi ((int8x8_t) __a, (int8x8_t) __b);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vtbl2_s8 (int8x8x2_t __a, int8x8_t __b)
{
  union { int8x8x2_t __i; __builtin_neon_ti __o; } __au = { __a };
  return (int8x8_t)__builtin_neon_vtbl2v8qi (__au.__o, __b);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vtbl2_u8 (uint8x8x2_t __a, uint8x8_t __b)
{
  union { uint8x8x2_t __i; __builtin_neon_ti __o; } __au = { __a };
  return (uint8x8_t)__builtin_neon_vtbl2v8qi (__au.__o, (int8x8_t) __b);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vtbl2_p8 (poly8x8x2_t __a, uint8x8_t __b)
{
  union { poly8x8x2_t __i; __builtin_neon_ti __o; } __au = { __a };
  return (poly8x8_t)__builtin_neon_vtbl2v8qi (__au.__o, (int8x8_t) __b);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vtbl3_s8 (int8x8x3_t __a, int8x8_t __b)
{
  union { int8x8x3_t __i; __builtin_neon_ei __o; } __au = { __a };
  return (int8x8_t)__builtin_neon_vtbl3v8qi (__au.__o, __b);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vtbl3_u8 (uint8x8x3_t __a, uint8x8_t __b)
{
  union { uint8x8x3_t __i; __builtin_neon_ei __o; } __au = { __a };
  return (uint8x8_t)__builtin_neon_vtbl3v8qi (__au.__o, (int8x8_t) __b);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vtbl3_p8 (poly8x8x3_t __a, uint8x8_t __b)
{
  union { poly8x8x3_t __i; __builtin_neon_ei __o; } __au = { __a };
  return (poly8x8_t)__builtin_neon_vtbl3v8qi (__au.__o, (int8x8_t) __b);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vtbl4_s8 (int8x8x4_t __a, int8x8_t __b)
{
  union { int8x8x4_t __i; __builtin_neon_oi __o; } __au = { __a };
  return (int8x8_t)__builtin_neon_vtbl4v8qi (__au.__o, __b);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vtbl4_u8 (uint8x8x4_t __a, uint8x8_t __b)
{
  union { uint8x8x4_t __i; __builtin_neon_oi __o; } __au = { __a };
  return (uint8x8_t)__builtin_neon_vtbl4v8qi (__au.__o, (int8x8_t) __b);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vtbl4_p8 (poly8x8x4_t __a, uint8x8_t __b)
{
  union { poly8x8x4_t __i; __builtin_neon_oi __o; } __au = { __a };
  return (poly8x8_t)__builtin_neon_vtbl4v8qi (__au.__o, (int8x8_t) __b);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vtbx1_s8 (int8x8_t __a, int8x8_t __b, int8x8_t __c)
{
  return (int8x8_t)__builtin_neon_vtbx1v8qi (__a, __b, __c);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vtbx1_u8 (uint8x8_t __a, uint8x8_t __b, uint8x8_t __c)
{
  return (uint8x8_t)__builtin_neon_vtbx1v8qi ((int8x8_t) __a, (int8x8_t) __b, (int8x8_t) __c);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vtbx1_p8 (poly8x8_t __a, poly8x8_t __b, uint8x8_t __c)
{
  return (poly8x8_t)__builtin_neon_vtbx1v8qi ((int8x8_t) __a, (int8x8_t) __b, (int8x8_t) __c);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vtbx2_s8 (int8x8_t __a, int8x8x2_t __b, int8x8_t __c)
{
  union { int8x8x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  return (int8x8_t)__builtin_neon_vtbx2v8qi (__a, __bu.__o, __c);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vtbx2_u8 (uint8x8_t __a, uint8x8x2_t __b, uint8x8_t __c)
{
  union { uint8x8x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  return (uint8x8_t)__builtin_neon_vtbx2v8qi ((int8x8_t) __a, __bu.__o, (int8x8_t) __c);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vtbx2_p8 (poly8x8_t __a, poly8x8x2_t __b, uint8x8_t __c)
{
  union { poly8x8x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  return (poly8x8_t)__builtin_neon_vtbx2v8qi ((int8x8_t) __a, __bu.__o, (int8x8_t) __c);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vtbx3_s8 (int8x8_t __a, int8x8x3_t __b, int8x8_t __c)
{
  union { int8x8x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  return (int8x8_t)__builtin_neon_vtbx3v8qi (__a, __bu.__o, __c);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vtbx3_u8 (uint8x8_t __a, uint8x8x3_t __b, uint8x8_t __c)
{
  union { uint8x8x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  return (uint8x8_t)__builtin_neon_vtbx3v8qi ((int8x8_t) __a, __bu.__o, (int8x8_t) __c);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vtbx3_p8 (poly8x8_t __a, poly8x8x3_t __b, uint8x8_t __c)
{
  union { poly8x8x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  return (poly8x8_t)__builtin_neon_vtbx3v8qi ((int8x8_t) __a, __bu.__o, (int8x8_t) __c);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vtbx4_s8 (int8x8_t __a, int8x8x4_t __b, int8x8_t __c)
{
  union { int8x8x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  return (int8x8_t)__builtin_neon_vtbx4v8qi (__a, __bu.__o, __c);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vtbx4_u8 (uint8x8_t __a, uint8x8x4_t __b, uint8x8_t __c)
{
  union { uint8x8x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  return (uint8x8_t)__builtin_neon_vtbx4v8qi ((int8x8_t) __a, __bu.__o, (int8x8_t) __c);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vtbx4_p8 (poly8x8_t __a, poly8x8x4_t __b, uint8x8_t __c)
{
  union { poly8x8x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  return (poly8x8_t)__builtin_neon_vtbx4v8qi ((int8x8_t) __a, __bu.__o, (int8x8_t) __c);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vmul_lane_s16 (int16x4_t __a, int16x4_t __b, const int __c)
{
  return (int16x4_t)__builtin_neon_vmul_lanev4hi (__a, __b, __c, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vmul_lane_s32 (int32x2_t __a, int32x2_t __b, const int __c)
{
  return (int32x2_t)__builtin_neon_vmul_lanev2si (__a, __b, __c, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vmul_lane_f32 (float32x2_t __a, float32x2_t __b, const int __c)
{
  return (float32x2_t)__builtin_neon_vmul_lanev2sf (__a, __b, __c, 3);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vmul_lane_u16 (uint16x4_t __a, uint16x4_t __b, const int __c)
{
  return (uint16x4_t)__builtin_neon_vmul_lanev4hi ((int16x4_t) __a, (int16x4_t) __b, __c, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vmul_lane_u32 (uint32x2_t __a, uint32x2_t __b, const int __c)
{
  return (uint32x2_t)__builtin_neon_vmul_lanev2si ((int32x2_t) __a, (int32x2_t) __b, __c, 0);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vmulq_lane_s16 (int16x8_t __a, int16x4_t __b, const int __c)
{
  return (int16x8_t)__builtin_neon_vmul_lanev8hi (__a, __b, __c, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmulq_lane_s32 (int32x4_t __a, int32x2_t __b, const int __c)
{
  return (int32x4_t)__builtin_neon_vmul_lanev4si (__a, __b, __c, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vmulq_lane_f32 (float32x4_t __a, float32x2_t __b, const int __c)
{
  return (float32x4_t)__builtin_neon_vmul_lanev4sf (__a, __b, __c, 3);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vmulq_lane_u16 (uint16x8_t __a, uint16x4_t __b, const int __c)
{
  return (uint16x8_t)__builtin_neon_vmul_lanev8hi ((int16x8_t) __a, (int16x4_t) __b, __c, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmulq_lane_u32 (uint32x4_t __a, uint32x2_t __b, const int __c)
{
  return (uint32x4_t)__builtin_neon_vmul_lanev4si ((int32x4_t) __a, (int32x2_t) __b, __c, 0);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vmla_lane_s16 (int16x4_t __a, int16x4_t __b, int16x4_t __c, const int __d)
{
  return (int16x4_t)__builtin_neon_vmla_lanev4hi (__a, __b, __c, __d, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vmla_lane_s32 (int32x2_t __a, int32x2_t __b, int32x2_t __c, const int __d)
{
  return (int32x2_t)__builtin_neon_vmla_lanev2si (__a, __b, __c, __d, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vmla_lane_f32 (float32x2_t __a, float32x2_t __b, float32x2_t __c, const int __d)
{
  return (float32x2_t)__builtin_neon_vmla_lanev2sf (__a, __b, __c, __d, 3);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vmla_lane_u16 (uint16x4_t __a, uint16x4_t __b, uint16x4_t __c, const int __d)
{
  return (uint16x4_t)__builtin_neon_vmla_lanev4hi ((int16x4_t) __a, (int16x4_t) __b, (int16x4_t) __c, __d, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vmla_lane_u32 (uint32x2_t __a, uint32x2_t __b, uint32x2_t __c, const int __d)
{
  return (uint32x2_t)__builtin_neon_vmla_lanev2si ((int32x2_t) __a, (int32x2_t) __b, (int32x2_t) __c, __d, 0);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vmlaq_lane_s16 (int16x8_t __a, int16x8_t __b, int16x4_t __c, const int __d)
{
  return (int16x8_t)__builtin_neon_vmla_lanev8hi (__a, __b, __c, __d, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmlaq_lane_s32 (int32x4_t __a, int32x4_t __b, int32x2_t __c, const int __d)
{
  return (int32x4_t)__builtin_neon_vmla_lanev4si (__a, __b, __c, __d, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vmlaq_lane_f32 (float32x4_t __a, float32x4_t __b, float32x2_t __c, const int __d)
{
  return (float32x4_t)__builtin_neon_vmla_lanev4sf (__a, __b, __c, __d, 3);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vmlaq_lane_u16 (uint16x8_t __a, uint16x8_t __b, uint16x4_t __c, const int __d)
{
  return (uint16x8_t)__builtin_neon_vmla_lanev8hi ((int16x8_t) __a, (int16x8_t) __b, (int16x4_t) __c, __d, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmlaq_lane_u32 (uint32x4_t __a, uint32x4_t __b, uint32x2_t __c, const int __d)
{
  return (uint32x4_t)__builtin_neon_vmla_lanev4si ((int32x4_t) __a, (int32x4_t) __b, (int32x2_t) __c, __d, 0);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmlal_lane_s16 (int32x4_t __a, int16x4_t __b, int16x4_t __c, const int __d)
{
  return (int32x4_t)__builtin_neon_vmlal_lanev4hi (__a, __b, __c, __d, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vmlal_lane_s32 (int64x2_t __a, int32x2_t __b, int32x2_t __c, const int __d)
{
  return (int64x2_t)__builtin_neon_vmlal_lanev2si (__a, __b, __c, __d, 1);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmlal_lane_u16 (uint32x4_t __a, uint16x4_t __b, uint16x4_t __c, const int __d)
{
  return (uint32x4_t)__builtin_neon_vmlal_lanev4hi ((int32x4_t) __a, (int16x4_t) __b, (int16x4_t) __c, __d, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vmlal_lane_u32 (uint64x2_t __a, uint32x2_t __b, uint32x2_t __c, const int __d)
{
  return (uint64x2_t)__builtin_neon_vmlal_lanev2si ((int64x2_t) __a, (int32x2_t) __b, (int32x2_t) __c, __d, 0);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqdmlal_lane_s16 (int32x4_t __a, int16x4_t __b, int16x4_t __c, const int __d)
{
  return (int32x4_t)__builtin_neon_vqdmlal_lanev4hi (__a, __b, __c, __d, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vqdmlal_lane_s32 (int64x2_t __a, int32x2_t __b, int32x2_t __c, const int __d)
{
  return (int64x2_t)__builtin_neon_vqdmlal_lanev2si (__a, __b, __c, __d, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vmls_lane_s16 (int16x4_t __a, int16x4_t __b, int16x4_t __c, const int __d)
{
  return (int16x4_t)__builtin_neon_vmls_lanev4hi (__a, __b, __c, __d, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vmls_lane_s32 (int32x2_t __a, int32x2_t __b, int32x2_t __c, const int __d)
{
  return (int32x2_t)__builtin_neon_vmls_lanev2si (__a, __b, __c, __d, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vmls_lane_f32 (float32x2_t __a, float32x2_t __b, float32x2_t __c, const int __d)
{
  return (float32x2_t)__builtin_neon_vmls_lanev2sf (__a, __b, __c, __d, 3);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vmls_lane_u16 (uint16x4_t __a, uint16x4_t __b, uint16x4_t __c, const int __d)
{
  return (uint16x4_t)__builtin_neon_vmls_lanev4hi ((int16x4_t) __a, (int16x4_t) __b, (int16x4_t) __c, __d, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vmls_lane_u32 (uint32x2_t __a, uint32x2_t __b, uint32x2_t __c, const int __d)
{
  return (uint32x2_t)__builtin_neon_vmls_lanev2si ((int32x2_t) __a, (int32x2_t) __b, (int32x2_t) __c, __d, 0);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vmlsq_lane_s16 (int16x8_t __a, int16x8_t __b, int16x4_t __c, const int __d)
{
  return (int16x8_t)__builtin_neon_vmls_lanev8hi (__a, __b, __c, __d, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmlsq_lane_s32 (int32x4_t __a, int32x4_t __b, int32x2_t __c, const int __d)
{
  return (int32x4_t)__builtin_neon_vmls_lanev4si (__a, __b, __c, __d, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vmlsq_lane_f32 (float32x4_t __a, float32x4_t __b, float32x2_t __c, const int __d)
{
  return (float32x4_t)__builtin_neon_vmls_lanev4sf (__a, __b, __c, __d, 3);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vmlsq_lane_u16 (uint16x8_t __a, uint16x8_t __b, uint16x4_t __c, const int __d)
{
  return (uint16x8_t)__builtin_neon_vmls_lanev8hi ((int16x8_t) __a, (int16x8_t) __b, (int16x4_t) __c, __d, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmlsq_lane_u32 (uint32x4_t __a, uint32x4_t __b, uint32x2_t __c, const int __d)
{
  return (uint32x4_t)__builtin_neon_vmls_lanev4si ((int32x4_t) __a, (int32x4_t) __b, (int32x2_t) __c, __d, 0);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmlsl_lane_s16 (int32x4_t __a, int16x4_t __b, int16x4_t __c, const int __d)
{
  return (int32x4_t)__builtin_neon_vmlsl_lanev4hi (__a, __b, __c, __d, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vmlsl_lane_s32 (int64x2_t __a, int32x2_t __b, int32x2_t __c, const int __d)
{
  return (int64x2_t)__builtin_neon_vmlsl_lanev2si (__a, __b, __c, __d, 1);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmlsl_lane_u16 (uint32x4_t __a, uint16x4_t __b, uint16x4_t __c, const int __d)
{
  return (uint32x4_t)__builtin_neon_vmlsl_lanev4hi ((int32x4_t) __a, (int16x4_t) __b, (int16x4_t) __c, __d, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vmlsl_lane_u32 (uint64x2_t __a, uint32x2_t __b, uint32x2_t __c, const int __d)
{
  return (uint64x2_t)__builtin_neon_vmlsl_lanev2si ((int64x2_t) __a, (int32x2_t) __b, (int32x2_t) __c, __d, 0);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqdmlsl_lane_s16 (int32x4_t __a, int16x4_t __b, int16x4_t __c, const int __d)
{
  return (int32x4_t)__builtin_neon_vqdmlsl_lanev4hi (__a, __b, __c, __d, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vqdmlsl_lane_s32 (int64x2_t __a, int32x2_t __b, int32x2_t __c, const int __d)
{
  return (int64x2_t)__builtin_neon_vqdmlsl_lanev2si (__a, __b, __c, __d, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmull_lane_s16 (int16x4_t __a, int16x4_t __b, const int __c)
{
  return (int32x4_t)__builtin_neon_vmull_lanev4hi (__a, __b, __c, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vmull_lane_s32 (int32x2_t __a, int32x2_t __b, const int __c)
{
  return (int64x2_t)__builtin_neon_vmull_lanev2si (__a, __b, __c, 1);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmull_lane_u16 (uint16x4_t __a, uint16x4_t __b, const int __c)
{
  return (uint32x4_t)__builtin_neon_vmull_lanev4hi ((int16x4_t) __a, (int16x4_t) __b, __c, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vmull_lane_u32 (uint32x2_t __a, uint32x2_t __b, const int __c)
{
  return (uint64x2_t)__builtin_neon_vmull_lanev2si ((int32x2_t) __a, (int32x2_t) __b, __c, 0);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqdmull_lane_s16 (int16x4_t __a, int16x4_t __b, const int __c)
{
  return (int32x4_t)__builtin_neon_vqdmull_lanev4hi (__a, __b, __c, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vqdmull_lane_s32 (int32x2_t __a, int32x2_t __b, const int __c)
{
  return (int64x2_t)__builtin_neon_vqdmull_lanev2si (__a, __b, __c, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vqdmulhq_lane_s16 (int16x8_t __a, int16x4_t __b, const int __c)
{
  return (int16x8_t)__builtin_neon_vqdmulh_lanev8hi (__a, __b, __c, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqdmulhq_lane_s32 (int32x4_t __a, int32x2_t __b, const int __c)
{
  return (int32x4_t)__builtin_neon_vqdmulh_lanev4si (__a, __b, __c, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vqdmulh_lane_s16 (int16x4_t __a, int16x4_t __b, const int __c)
{
  return (int16x4_t)__builtin_neon_vqdmulh_lanev4hi (__a, __b, __c, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vqdmulh_lane_s32 (int32x2_t __a, int32x2_t __b, const int __c)
{
  return (int32x2_t)__builtin_neon_vqdmulh_lanev2si (__a, __b, __c, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vqrdmulhq_lane_s16 (int16x8_t __a, int16x4_t __b, const int __c)
{
  return (int16x8_t)__builtin_neon_vqdmulh_lanev8hi (__a, __b, __c, 5);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqrdmulhq_lane_s32 (int32x4_t __a, int32x2_t __b, const int __c)
{
  return (int32x4_t)__builtin_neon_vqdmulh_lanev4si (__a, __b, __c, 5);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vqrdmulh_lane_s16 (int16x4_t __a, int16x4_t __b, const int __c)
{
  return (int16x4_t)__builtin_neon_vqdmulh_lanev4hi (__a, __b, __c, 5);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vqrdmulh_lane_s32 (int32x2_t __a, int32x2_t __b, const int __c)
{
  return (int32x2_t)__builtin_neon_vqdmulh_lanev2si (__a, __b, __c, 5);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vmul_n_s16 (int16x4_t __a, int16_t __b)
{
  return (int16x4_t)__builtin_neon_vmul_nv4hi (__a, (__builtin_neon_hi) __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vmul_n_s32 (int32x2_t __a, int32_t __b)
{
  return (int32x2_t)__builtin_neon_vmul_nv2si (__a, (__builtin_neon_si) __b, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vmul_n_f32 (float32x2_t __a, float32_t __b)
{
  return (float32x2_t)__builtin_neon_vmul_nv2sf (__a, (__builtin_neon_sf) __b, 3);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vmul_n_u16 (uint16x4_t __a, uint16_t __b)
{
  return (uint16x4_t)__builtin_neon_vmul_nv4hi ((int16x4_t) __a, (__builtin_neon_hi) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vmul_n_u32 (uint32x2_t __a, uint32_t __b)
{
  return (uint32x2_t)__builtin_neon_vmul_nv2si ((int32x2_t) __a, (__builtin_neon_si) __b, 0);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vmulq_n_s16 (int16x8_t __a, int16_t __b)
{
  return (int16x8_t)__builtin_neon_vmul_nv8hi (__a, (__builtin_neon_hi) __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmulq_n_s32 (int32x4_t __a, int32_t __b)
{
  return (int32x4_t)__builtin_neon_vmul_nv4si (__a, (__builtin_neon_si) __b, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vmulq_n_f32 (float32x4_t __a, float32_t __b)
{
  return (float32x4_t)__builtin_neon_vmul_nv4sf (__a, (__builtin_neon_sf) __b, 3);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vmulq_n_u16 (uint16x8_t __a, uint16_t __b)
{
  return (uint16x8_t)__builtin_neon_vmul_nv8hi ((int16x8_t) __a, (__builtin_neon_hi) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmulq_n_u32 (uint32x4_t __a, uint32_t __b)
{
  return (uint32x4_t)__builtin_neon_vmul_nv4si ((int32x4_t) __a, (__builtin_neon_si) __b, 0);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmull_n_s16 (int16x4_t __a, int16_t __b)
{
  return (int32x4_t)__builtin_neon_vmull_nv4hi (__a, (__builtin_neon_hi) __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vmull_n_s32 (int32x2_t __a, int32_t __b)
{
  return (int64x2_t)__builtin_neon_vmull_nv2si (__a, (__builtin_neon_si) __b, 1);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmull_n_u16 (uint16x4_t __a, uint16_t __b)
{
  return (uint32x4_t)__builtin_neon_vmull_nv4hi ((int16x4_t) __a, (__builtin_neon_hi) __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vmull_n_u32 (uint32x2_t __a, uint32_t __b)
{
  return (uint64x2_t)__builtin_neon_vmull_nv2si ((int32x2_t) __a, (__builtin_neon_si) __b, 0);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqdmull_n_s16 (int16x4_t __a, int16_t __b)
{
  return (int32x4_t)__builtin_neon_vqdmull_nv4hi (__a, (__builtin_neon_hi) __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vqdmull_n_s32 (int32x2_t __a, int32_t __b)
{
  return (int64x2_t)__builtin_neon_vqdmull_nv2si (__a, (__builtin_neon_si) __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vqdmulhq_n_s16 (int16x8_t __a, int16_t __b)
{
  return (int16x8_t)__builtin_neon_vqdmulh_nv8hi (__a, (__builtin_neon_hi) __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqdmulhq_n_s32 (int32x4_t __a, int32_t __b)
{
  return (int32x4_t)__builtin_neon_vqdmulh_nv4si (__a, (__builtin_neon_si) __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vqdmulh_n_s16 (int16x4_t __a, int16_t __b)
{
  return (int16x4_t)__builtin_neon_vqdmulh_nv4hi (__a, (__builtin_neon_hi) __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vqdmulh_n_s32 (int32x2_t __a, int32_t __b)
{
  return (int32x2_t)__builtin_neon_vqdmulh_nv2si (__a, (__builtin_neon_si) __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vqrdmulhq_n_s16 (int16x8_t __a, int16_t __b)
{
  return (int16x8_t)__builtin_neon_vqdmulh_nv8hi (__a, (__builtin_neon_hi) __b, 5);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqrdmulhq_n_s32 (int32x4_t __a, int32_t __b)
{
  return (int32x4_t)__builtin_neon_vqdmulh_nv4si (__a, (__builtin_neon_si) __b, 5);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vqrdmulh_n_s16 (int16x4_t __a, int16_t __b)
{
  return (int16x4_t)__builtin_neon_vqdmulh_nv4hi (__a, (__builtin_neon_hi) __b, 5);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vqrdmulh_n_s32 (int32x2_t __a, int32_t __b)
{
  return (int32x2_t)__builtin_neon_vqdmulh_nv2si (__a, (__builtin_neon_si) __b, 5);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vmla_n_s16 (int16x4_t __a, int16x4_t __b, int16_t __c)
{
  return (int16x4_t)__builtin_neon_vmla_nv4hi (__a, __b, (__builtin_neon_hi) __c, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vmla_n_s32 (int32x2_t __a, int32x2_t __b, int32_t __c)
{
  return (int32x2_t)__builtin_neon_vmla_nv2si (__a, __b, (__builtin_neon_si) __c, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vmla_n_f32 (float32x2_t __a, float32x2_t __b, float32_t __c)
{
  return (float32x2_t)__builtin_neon_vmla_nv2sf (__a, __b, (__builtin_neon_sf) __c, 3);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vmla_n_u16 (uint16x4_t __a, uint16x4_t __b, uint16_t __c)
{
  return (uint16x4_t)__builtin_neon_vmla_nv4hi ((int16x4_t) __a, (int16x4_t) __b, (__builtin_neon_hi) __c, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vmla_n_u32 (uint32x2_t __a, uint32x2_t __b, uint32_t __c)
{
  return (uint32x2_t)__builtin_neon_vmla_nv2si ((int32x2_t) __a, (int32x2_t) __b, (__builtin_neon_si) __c, 0);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vmlaq_n_s16 (int16x8_t __a, int16x8_t __b, int16_t __c)
{
  return (int16x8_t)__builtin_neon_vmla_nv8hi (__a, __b, (__builtin_neon_hi) __c, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmlaq_n_s32 (int32x4_t __a, int32x4_t __b, int32_t __c)
{
  return (int32x4_t)__builtin_neon_vmla_nv4si (__a, __b, (__builtin_neon_si) __c, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vmlaq_n_f32 (float32x4_t __a, float32x4_t __b, float32_t __c)
{
  return (float32x4_t)__builtin_neon_vmla_nv4sf (__a, __b, (__builtin_neon_sf) __c, 3);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vmlaq_n_u16 (uint16x8_t __a, uint16x8_t __b, uint16_t __c)
{
  return (uint16x8_t)__builtin_neon_vmla_nv8hi ((int16x8_t) __a, (int16x8_t) __b, (__builtin_neon_hi) __c, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmlaq_n_u32 (uint32x4_t __a, uint32x4_t __b, uint32_t __c)
{
  return (uint32x4_t)__builtin_neon_vmla_nv4si ((int32x4_t) __a, (int32x4_t) __b, (__builtin_neon_si) __c, 0);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmlal_n_s16 (int32x4_t __a, int16x4_t __b, int16_t __c)
{
  return (int32x4_t)__builtin_neon_vmlal_nv4hi (__a, __b, (__builtin_neon_hi) __c, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vmlal_n_s32 (int64x2_t __a, int32x2_t __b, int32_t __c)
{
  return (int64x2_t)__builtin_neon_vmlal_nv2si (__a, __b, (__builtin_neon_si) __c, 1);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmlal_n_u16 (uint32x4_t __a, uint16x4_t __b, uint16_t __c)
{
  return (uint32x4_t)__builtin_neon_vmlal_nv4hi ((int32x4_t) __a, (int16x4_t) __b, (__builtin_neon_hi) __c, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vmlal_n_u32 (uint64x2_t __a, uint32x2_t __b, uint32_t __c)
{
  return (uint64x2_t)__builtin_neon_vmlal_nv2si ((int64x2_t) __a, (int32x2_t) __b, (__builtin_neon_si) __c, 0);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqdmlal_n_s16 (int32x4_t __a, int16x4_t __b, int16_t __c)
{
  return (int32x4_t)__builtin_neon_vqdmlal_nv4hi (__a, __b, (__builtin_neon_hi) __c, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vqdmlal_n_s32 (int64x2_t __a, int32x2_t __b, int32_t __c)
{
  return (int64x2_t)__builtin_neon_vqdmlal_nv2si (__a, __b, (__builtin_neon_si) __c, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vmls_n_s16 (int16x4_t __a, int16x4_t __b, int16_t __c)
{
  return (int16x4_t)__builtin_neon_vmls_nv4hi (__a, __b, (__builtin_neon_hi) __c, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vmls_n_s32 (int32x2_t __a, int32x2_t __b, int32_t __c)
{
  return (int32x2_t)__builtin_neon_vmls_nv2si (__a, __b, (__builtin_neon_si) __c, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vmls_n_f32 (float32x2_t __a, float32x2_t __b, float32_t __c)
{
  return (float32x2_t)__builtin_neon_vmls_nv2sf (__a, __b, (__builtin_neon_sf) __c, 3);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vmls_n_u16 (uint16x4_t __a, uint16x4_t __b, uint16_t __c)
{
  return (uint16x4_t)__builtin_neon_vmls_nv4hi ((int16x4_t) __a, (int16x4_t) __b, (__builtin_neon_hi) __c, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vmls_n_u32 (uint32x2_t __a, uint32x2_t __b, uint32_t __c)
{
  return (uint32x2_t)__builtin_neon_vmls_nv2si ((int32x2_t) __a, (int32x2_t) __b, (__builtin_neon_si) __c, 0);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vmlsq_n_s16 (int16x8_t __a, int16x8_t __b, int16_t __c)
{
  return (int16x8_t)__builtin_neon_vmls_nv8hi (__a, __b, (__builtin_neon_hi) __c, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmlsq_n_s32 (int32x4_t __a, int32x4_t __b, int32_t __c)
{
  return (int32x4_t)__builtin_neon_vmls_nv4si (__a, __b, (__builtin_neon_si) __c, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vmlsq_n_f32 (float32x4_t __a, float32x4_t __b, float32_t __c)
{
  return (float32x4_t)__builtin_neon_vmls_nv4sf (__a, __b, (__builtin_neon_sf) __c, 3);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vmlsq_n_u16 (uint16x8_t __a, uint16x8_t __b, uint16_t __c)
{
  return (uint16x8_t)__builtin_neon_vmls_nv8hi ((int16x8_t) __a, (int16x8_t) __b, (__builtin_neon_hi) __c, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmlsq_n_u32 (uint32x4_t __a, uint32x4_t __b, uint32_t __c)
{
  return (uint32x4_t)__builtin_neon_vmls_nv4si ((int32x4_t) __a, (int32x4_t) __b, (__builtin_neon_si) __c, 0);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vmlsl_n_s16 (int32x4_t __a, int16x4_t __b, int16_t __c)
{
  return (int32x4_t)__builtin_neon_vmlsl_nv4hi (__a, __b, (__builtin_neon_hi) __c, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vmlsl_n_s32 (int64x2_t __a, int32x2_t __b, int32_t __c)
{
  return (int64x2_t)__builtin_neon_vmlsl_nv2si (__a, __b, (__builtin_neon_si) __c, 1);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vmlsl_n_u16 (uint32x4_t __a, uint16x4_t __b, uint16_t __c)
{
  return (uint32x4_t)__builtin_neon_vmlsl_nv4hi ((int32x4_t) __a, (int16x4_t) __b, (__builtin_neon_hi) __c, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vmlsl_n_u32 (uint64x2_t __a, uint32x2_t __b, uint32_t __c)
{
  return (uint64x2_t)__builtin_neon_vmlsl_nv2si ((int64x2_t) __a, (int32x2_t) __b, (__builtin_neon_si) __c, 0);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vqdmlsl_n_s16 (int32x4_t __a, int16x4_t __b, int16_t __c)
{
  return (int32x4_t)__builtin_neon_vqdmlsl_nv4hi (__a, __b, (__builtin_neon_hi) __c, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vqdmlsl_n_s32 (int64x2_t __a, int32x2_t __b, int32_t __c)
{
  return (int64x2_t)__builtin_neon_vqdmlsl_nv2si (__a, __b, (__builtin_neon_si) __c, 1);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vext_s8 (int8x8_t __a, int8x8_t __b, const int __c)
{
  return (int8x8_t)__builtin_neon_vextv8qi (__a, __b, __c);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vext_s16 (int16x4_t __a, int16x4_t __b, const int __c)
{
  return (int16x4_t)__builtin_neon_vextv4hi (__a, __b, __c);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vext_s32 (int32x2_t __a, int32x2_t __b, const int __c)
{
  return (int32x2_t)__builtin_neon_vextv2si (__a, __b, __c);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vext_s64 (int64x1_t __a, int64x1_t __b, const int __c)
{
  return (int64x1_t)__builtin_neon_vextdi (__a, __b, __c);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vext_f32 (float32x2_t __a, float32x2_t __b, const int __c)
{
  return (float32x2_t)__builtin_neon_vextv2sf (__a, __b, __c);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vext_u8 (uint8x8_t __a, uint8x8_t __b, const int __c)
{
  return (uint8x8_t)__builtin_neon_vextv8qi ((int8x8_t) __a, (int8x8_t) __b, __c);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vext_u16 (uint16x4_t __a, uint16x4_t __b, const int __c)
{
  return (uint16x4_t)__builtin_neon_vextv4hi ((int16x4_t) __a, (int16x4_t) __b, __c);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vext_u32 (uint32x2_t __a, uint32x2_t __b, const int __c)
{
  return (uint32x2_t)__builtin_neon_vextv2si ((int32x2_t) __a, (int32x2_t) __b, __c);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vext_u64 (uint64x1_t __a, uint64x1_t __b, const int __c)
{
  return (uint64x1_t)__builtin_neon_vextdi ((int64x1_t) __a, (int64x1_t) __b, __c);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vext_p8 (poly8x8_t __a, poly8x8_t __b, const int __c)
{
  return (poly8x8_t)__builtin_neon_vextv8qi ((int8x8_t) __a, (int8x8_t) __b, __c);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vext_p16 (poly16x4_t __a, poly16x4_t __b, const int __c)
{
  return (poly16x4_t)__builtin_neon_vextv4hi ((int16x4_t) __a, (int16x4_t) __b, __c);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vextq_s8 (int8x16_t __a, int8x16_t __b, const int __c)
{
  return (int8x16_t)__builtin_neon_vextv16qi (__a, __b, __c);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vextq_s16 (int16x8_t __a, int16x8_t __b, const int __c)
{
  return (int16x8_t)__builtin_neon_vextv8hi (__a, __b, __c);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vextq_s32 (int32x4_t __a, int32x4_t __b, const int __c)
{
  return (int32x4_t)__builtin_neon_vextv4si (__a, __b, __c);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vextq_s64 (int64x2_t __a, int64x2_t __b, const int __c)
{
  return (int64x2_t)__builtin_neon_vextv2di (__a, __b, __c);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vextq_f32 (float32x4_t __a, float32x4_t __b, const int __c)
{
  return (float32x4_t)__builtin_neon_vextv4sf (__a, __b, __c);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vextq_u8 (uint8x16_t __a, uint8x16_t __b, const int __c)
{
  return (uint8x16_t)__builtin_neon_vextv16qi ((int8x16_t) __a, (int8x16_t) __b, __c);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vextq_u16 (uint16x8_t __a, uint16x8_t __b, const int __c)
{
  return (uint16x8_t)__builtin_neon_vextv8hi ((int16x8_t) __a, (int16x8_t) __b, __c);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vextq_u32 (uint32x4_t __a, uint32x4_t __b, const int __c)
{
  return (uint32x4_t)__builtin_neon_vextv4si ((int32x4_t) __a, (int32x4_t) __b, __c);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vextq_u64 (uint64x2_t __a, uint64x2_t __b, const int __c)
{
  return (uint64x2_t)__builtin_neon_vextv2di ((int64x2_t) __a, (int64x2_t) __b, __c);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vextq_p8 (poly8x16_t __a, poly8x16_t __b, const int __c)
{
  return (poly8x16_t)__builtin_neon_vextv16qi ((int8x16_t) __a, (int8x16_t) __b, __c);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vextq_p16 (poly16x8_t __a, poly16x8_t __b, const int __c)
{
  return (poly16x8_t)__builtin_neon_vextv8hi ((int16x8_t) __a, (int16x8_t) __b, __c);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vrev64_s8 (int8x8_t __a)
{
  return (int8x8_t)__builtin_neon_vrev64v8qi (__a, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vrev64_s16 (int16x4_t __a)
{
  return (int16x4_t)__builtin_neon_vrev64v4hi (__a, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vrev64_s32 (int32x2_t __a)
{
  return (int32x2_t)__builtin_neon_vrev64v2si (__a, 1);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vrev64_f32 (float32x2_t __a)
{
  return (float32x2_t)__builtin_neon_vrev64v2sf (__a, 3);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vrev64_u8 (uint8x8_t __a)
{
  return (uint8x8_t)__builtin_neon_vrev64v8qi ((int8x8_t) __a, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vrev64_u16 (uint16x4_t __a)
{
  return (uint16x4_t)__builtin_neon_vrev64v4hi ((int16x4_t) __a, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vrev64_u32 (uint32x2_t __a)
{
  return (uint32x2_t)__builtin_neon_vrev64v2si ((int32x2_t) __a, 0);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vrev64_p8 (poly8x8_t __a)
{
  return (poly8x8_t)__builtin_neon_vrev64v8qi ((int8x8_t) __a, 2);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vrev64_p16 (poly16x4_t __a)
{
  return (poly16x4_t)__builtin_neon_vrev64v4hi ((int16x4_t) __a, 2);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vrev64q_s8 (int8x16_t __a)
{
  return (int8x16_t)__builtin_neon_vrev64v16qi (__a, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vrev64q_s16 (int16x8_t __a)
{
  return (int16x8_t)__builtin_neon_vrev64v8hi (__a, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vrev64q_s32 (int32x4_t __a)
{
  return (int32x4_t)__builtin_neon_vrev64v4si (__a, 1);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vrev64q_f32 (float32x4_t __a)
{
  return (float32x4_t)__builtin_neon_vrev64v4sf (__a, 3);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vrev64q_u8 (uint8x16_t __a)
{
  return (uint8x16_t)__builtin_neon_vrev64v16qi ((int8x16_t) __a, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vrev64q_u16 (uint16x8_t __a)
{
  return (uint16x8_t)__builtin_neon_vrev64v8hi ((int16x8_t) __a, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vrev64q_u32 (uint32x4_t __a)
{
  return (uint32x4_t)__builtin_neon_vrev64v4si ((int32x4_t) __a, 0);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vrev64q_p8 (poly8x16_t __a)
{
  return (poly8x16_t)__builtin_neon_vrev64v16qi ((int8x16_t) __a, 2);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vrev64q_p16 (poly16x8_t __a)
{
  return (poly16x8_t)__builtin_neon_vrev64v8hi ((int16x8_t) __a, 2);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vrev32_s8 (int8x8_t __a)
{
  return (int8x8_t)__builtin_neon_vrev32v8qi (__a, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vrev32_s16 (int16x4_t __a)
{
  return (int16x4_t)__builtin_neon_vrev32v4hi (__a, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vrev32_u8 (uint8x8_t __a)
{
  return (uint8x8_t)__builtin_neon_vrev32v8qi ((int8x8_t) __a, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vrev32_u16 (uint16x4_t __a)
{
  return (uint16x4_t)__builtin_neon_vrev32v4hi ((int16x4_t) __a, 0);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vrev32_p8 (poly8x8_t __a)
{
  return (poly8x8_t)__builtin_neon_vrev32v8qi ((int8x8_t) __a, 2);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vrev32_p16 (poly16x4_t __a)
{
  return (poly16x4_t)__builtin_neon_vrev32v4hi ((int16x4_t) __a, 2);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vrev32q_s8 (int8x16_t __a)
{
  return (int8x16_t)__builtin_neon_vrev32v16qi (__a, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vrev32q_s16 (int16x8_t __a)
{
  return (int16x8_t)__builtin_neon_vrev32v8hi (__a, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vrev32q_u8 (uint8x16_t __a)
{
  return (uint8x16_t)__builtin_neon_vrev32v16qi ((int8x16_t) __a, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vrev32q_u16 (uint16x8_t __a)
{
  return (uint16x8_t)__builtin_neon_vrev32v8hi ((int16x8_t) __a, 0);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vrev32q_p8 (poly8x16_t __a)
{
  return (poly8x16_t)__builtin_neon_vrev32v16qi ((int8x16_t) __a, 2);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vrev32q_p16 (poly16x8_t __a)
{
  return (poly16x8_t)__builtin_neon_vrev32v8hi ((int16x8_t) __a, 2);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vrev16_s8 (int8x8_t __a)
{
  return (int8x8_t)__builtin_neon_vrev16v8qi (__a, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vrev16_u8 (uint8x8_t __a)
{
  return (uint8x8_t)__builtin_neon_vrev16v8qi ((int8x8_t) __a, 0);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vrev16_p8 (poly8x8_t __a)
{
  return (poly8x8_t)__builtin_neon_vrev16v8qi ((int8x8_t) __a, 2);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vrev16q_s8 (int8x16_t __a)
{
  return (int8x16_t)__builtin_neon_vrev16v16qi (__a, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vrev16q_u8 (uint8x16_t __a)
{
  return (uint8x16_t)__builtin_neon_vrev16v16qi ((int8x16_t) __a, 0);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vrev16q_p8 (poly8x16_t __a)
{
  return (poly8x16_t)__builtin_neon_vrev16v16qi ((int8x16_t) __a, 2);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vbsl_s8 (uint8x8_t __a, int8x8_t __b, int8x8_t __c)
{
  return (int8x8_t)__builtin_neon_vbslv8qi ((int8x8_t) __a, __b, __c);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vbsl_s16 (uint16x4_t __a, int16x4_t __b, int16x4_t __c)
{
  return (int16x4_t)__builtin_neon_vbslv4hi ((int16x4_t) __a, __b, __c);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vbsl_s32 (uint32x2_t __a, int32x2_t __b, int32x2_t __c)
{
  return (int32x2_t)__builtin_neon_vbslv2si ((int32x2_t) __a, __b, __c);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vbsl_s64 (uint64x1_t __a, int64x1_t __b, int64x1_t __c)
{
  return (int64x1_t)__builtin_neon_vbsldi ((int64x1_t) __a, __b, __c);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vbsl_f32 (uint32x2_t __a, float32x2_t __b, float32x2_t __c)
{
  return (float32x2_t)__builtin_neon_vbslv2sf ((int32x2_t) __a, __b, __c);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vbsl_u8 (uint8x8_t __a, uint8x8_t __b, uint8x8_t __c)
{
  return (uint8x8_t)__builtin_neon_vbslv8qi ((int8x8_t) __a, (int8x8_t) __b, (int8x8_t) __c);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vbsl_u16 (uint16x4_t __a, uint16x4_t __b, uint16x4_t __c)
{
  return (uint16x4_t)__builtin_neon_vbslv4hi ((int16x4_t) __a, (int16x4_t) __b, (int16x4_t) __c);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vbsl_u32 (uint32x2_t __a, uint32x2_t __b, uint32x2_t __c)
{
  return (uint32x2_t)__builtin_neon_vbslv2si ((int32x2_t) __a, (int32x2_t) __b, (int32x2_t) __c);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vbsl_u64 (uint64x1_t __a, uint64x1_t __b, uint64x1_t __c)
{
  return (uint64x1_t)__builtin_neon_vbsldi ((int64x1_t) __a, (int64x1_t) __b, (int64x1_t) __c);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vbsl_p8 (uint8x8_t __a, poly8x8_t __b, poly8x8_t __c)
{
  return (poly8x8_t)__builtin_neon_vbslv8qi ((int8x8_t) __a, (int8x8_t) __b, (int8x8_t) __c);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vbsl_p16 (uint16x4_t __a, poly16x4_t __b, poly16x4_t __c)
{
  return (poly16x4_t)__builtin_neon_vbslv4hi ((int16x4_t) __a, (int16x4_t) __b, (int16x4_t) __c);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vbslq_s8 (uint8x16_t __a, int8x16_t __b, int8x16_t __c)
{
  return (int8x16_t)__builtin_neon_vbslv16qi ((int8x16_t) __a, __b, __c);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vbslq_s16 (uint16x8_t __a, int16x8_t __b, int16x8_t __c)
{
  return (int16x8_t)__builtin_neon_vbslv8hi ((int16x8_t) __a, __b, __c);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vbslq_s32 (uint32x4_t __a, int32x4_t __b, int32x4_t __c)
{
  return (int32x4_t)__builtin_neon_vbslv4si ((int32x4_t) __a, __b, __c);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vbslq_s64 (uint64x2_t __a, int64x2_t __b, int64x2_t __c)
{
  return (int64x2_t)__builtin_neon_vbslv2di ((int64x2_t) __a, __b, __c);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vbslq_f32 (uint32x4_t __a, float32x4_t __b, float32x4_t __c)
{
  return (float32x4_t)__builtin_neon_vbslv4sf ((int32x4_t) __a, __b, __c);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vbslq_u8 (uint8x16_t __a, uint8x16_t __b, uint8x16_t __c)
{
  return (uint8x16_t)__builtin_neon_vbslv16qi ((int8x16_t) __a, (int8x16_t) __b, (int8x16_t) __c);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vbslq_u16 (uint16x8_t __a, uint16x8_t __b, uint16x8_t __c)
{
  return (uint16x8_t)__builtin_neon_vbslv8hi ((int16x8_t) __a, (int16x8_t) __b, (int16x8_t) __c);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vbslq_u32 (uint32x4_t __a, uint32x4_t __b, uint32x4_t __c)
{
  return (uint32x4_t)__builtin_neon_vbslv4si ((int32x4_t) __a, (int32x4_t) __b, (int32x4_t) __c);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vbslq_u64 (uint64x2_t __a, uint64x2_t __b, uint64x2_t __c)
{
  return (uint64x2_t)__builtin_neon_vbslv2di ((int64x2_t) __a, (int64x2_t) __b, (int64x2_t) __c);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vbslq_p8 (uint8x16_t __a, poly8x16_t __b, poly8x16_t __c)
{
  return (poly8x16_t)__builtin_neon_vbslv16qi ((int8x16_t) __a, (int8x16_t) __b, (int8x16_t) __c);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vbslq_p16 (uint16x8_t __a, poly16x8_t __b, poly16x8_t __c)
{
  return (poly16x8_t)__builtin_neon_vbslv8hi ((int16x8_t) __a, (int16x8_t) __b, (int16x8_t) __c);
}

__extension__ static __inline int8x8x2_t __attribute__ ((__always_inline__))
vtrn_s8 (int8x8_t __a, int8x8_t __b)
{
  int8x8x2_t __rv;
  __builtin_neon_vtrnv8qi (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline int16x4x2_t __attribute__ ((__always_inline__))
vtrn_s16 (int16x4_t __a, int16x4_t __b)
{
  int16x4x2_t __rv;
  __builtin_neon_vtrnv4hi (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline int32x2x2_t __attribute__ ((__always_inline__))
vtrn_s32 (int32x2_t __a, int32x2_t __b)
{
  int32x2x2_t __rv;
  __builtin_neon_vtrnv2si (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline float32x2x2_t __attribute__ ((__always_inline__))
vtrn_f32 (float32x2_t __a, float32x2_t __b)
{
  float32x2x2_t __rv;
  __builtin_neon_vtrnv2sf (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline uint8x8x2_t __attribute__ ((__always_inline__))
vtrn_u8 (uint8x8_t __a, uint8x8_t __b)
{
  uint8x8x2_t __rv;
  __builtin_neon_vtrnv8qi ((int8x8_t *) &__rv.val[0], (int8x8_t) __a, (int8x8_t) __b);
  return __rv;
}

__extension__ static __inline uint16x4x2_t __attribute__ ((__always_inline__))
vtrn_u16 (uint16x4_t __a, uint16x4_t __b)
{
  uint16x4x2_t __rv;
  __builtin_neon_vtrnv4hi ((int16x4_t *) &__rv.val[0], (int16x4_t) __a, (int16x4_t) __b);
  return __rv;
}

__extension__ static __inline uint32x2x2_t __attribute__ ((__always_inline__))
vtrn_u32 (uint32x2_t __a, uint32x2_t __b)
{
  uint32x2x2_t __rv;
  __builtin_neon_vtrnv2si ((int32x2_t *) &__rv.val[0], (int32x2_t) __a, (int32x2_t) __b);
  return __rv;
}

__extension__ static __inline poly8x8x2_t __attribute__ ((__always_inline__))
vtrn_p8 (poly8x8_t __a, poly8x8_t __b)
{
  poly8x8x2_t __rv;
  __builtin_neon_vtrnv8qi ((int8x8_t *) &__rv.val[0], (int8x8_t) __a, (int8x8_t) __b);
  return __rv;
}

__extension__ static __inline poly16x4x2_t __attribute__ ((__always_inline__))
vtrn_p16 (poly16x4_t __a, poly16x4_t __b)
{
  poly16x4x2_t __rv;
  __builtin_neon_vtrnv4hi ((int16x4_t *) &__rv.val[0], (int16x4_t) __a, (int16x4_t) __b);
  return __rv;
}

__extension__ static __inline int8x16x2_t __attribute__ ((__always_inline__))
vtrnq_s8 (int8x16_t __a, int8x16_t __b)
{
  int8x16x2_t __rv;
  __builtin_neon_vtrnv16qi (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline int16x8x2_t __attribute__ ((__always_inline__))
vtrnq_s16 (int16x8_t __a, int16x8_t __b)
{
  int16x8x2_t __rv;
  __builtin_neon_vtrnv8hi (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline int32x4x2_t __attribute__ ((__always_inline__))
vtrnq_s32 (int32x4_t __a, int32x4_t __b)
{
  int32x4x2_t __rv;
  __builtin_neon_vtrnv4si (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline float32x4x2_t __attribute__ ((__always_inline__))
vtrnq_f32 (float32x4_t __a, float32x4_t __b)
{
  float32x4x2_t __rv;
  __builtin_neon_vtrnv4sf (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline uint8x16x2_t __attribute__ ((__always_inline__))
vtrnq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  uint8x16x2_t __rv;
  __builtin_neon_vtrnv16qi ((int8x16_t *) &__rv.val[0], (int8x16_t) __a, (int8x16_t) __b);
  return __rv;
}

__extension__ static __inline uint16x8x2_t __attribute__ ((__always_inline__))
vtrnq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  uint16x8x2_t __rv;
  __builtin_neon_vtrnv8hi ((int16x8_t *) &__rv.val[0], (int16x8_t) __a, (int16x8_t) __b);
  return __rv;
}

__extension__ static __inline uint32x4x2_t __attribute__ ((__always_inline__))
vtrnq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  uint32x4x2_t __rv;
  __builtin_neon_vtrnv4si ((int32x4_t *) &__rv.val[0], (int32x4_t) __a, (int32x4_t) __b);
  return __rv;
}

__extension__ static __inline poly8x16x2_t __attribute__ ((__always_inline__))
vtrnq_p8 (poly8x16_t __a, poly8x16_t __b)
{
  poly8x16x2_t __rv;
  __builtin_neon_vtrnv16qi ((int8x16_t *) &__rv.val[0], (int8x16_t) __a, (int8x16_t) __b);
  return __rv;
}

__extension__ static __inline poly16x8x2_t __attribute__ ((__always_inline__))
vtrnq_p16 (poly16x8_t __a, poly16x8_t __b)
{
  poly16x8x2_t __rv;
  __builtin_neon_vtrnv8hi ((int16x8_t *) &__rv.val[0], (int16x8_t) __a, (int16x8_t) __b);
  return __rv;
}

__extension__ static __inline int8x8x2_t __attribute__ ((__always_inline__))
vzip_s8 (int8x8_t __a, int8x8_t __b)
{
  int8x8x2_t __rv;
  __builtin_neon_vzipv8qi (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline int16x4x2_t __attribute__ ((__always_inline__))
vzip_s16 (int16x4_t __a, int16x4_t __b)
{
  int16x4x2_t __rv;
  __builtin_neon_vzipv4hi (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline int32x2x2_t __attribute__ ((__always_inline__))
vzip_s32 (int32x2_t __a, int32x2_t __b)
{
  int32x2x2_t __rv;
  __builtin_neon_vzipv2si (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline float32x2x2_t __attribute__ ((__always_inline__))
vzip_f32 (float32x2_t __a, float32x2_t __b)
{
  float32x2x2_t __rv;
  __builtin_neon_vzipv2sf (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline uint8x8x2_t __attribute__ ((__always_inline__))
vzip_u8 (uint8x8_t __a, uint8x8_t __b)
{
  uint8x8x2_t __rv;
  __builtin_neon_vzipv8qi ((int8x8_t *) &__rv.val[0], (int8x8_t) __a, (int8x8_t) __b);
  return __rv;
}

__extension__ static __inline uint16x4x2_t __attribute__ ((__always_inline__))
vzip_u16 (uint16x4_t __a, uint16x4_t __b)
{
  uint16x4x2_t __rv;
  __builtin_neon_vzipv4hi ((int16x4_t *) &__rv.val[0], (int16x4_t) __a, (int16x4_t) __b);
  return __rv;
}

__extension__ static __inline uint32x2x2_t __attribute__ ((__always_inline__))
vzip_u32 (uint32x2_t __a, uint32x2_t __b)
{
  uint32x2x2_t __rv;
  __builtin_neon_vzipv2si ((int32x2_t *) &__rv.val[0], (int32x2_t) __a, (int32x2_t) __b);
  return __rv;
}

__extension__ static __inline poly8x8x2_t __attribute__ ((__always_inline__))
vzip_p8 (poly8x8_t __a, poly8x8_t __b)
{
  poly8x8x2_t __rv;
  __builtin_neon_vzipv8qi ((int8x8_t *) &__rv.val[0], (int8x8_t) __a, (int8x8_t) __b);
  return __rv;
}

__extension__ static __inline poly16x4x2_t __attribute__ ((__always_inline__))
vzip_p16 (poly16x4_t __a, poly16x4_t __b)
{
  poly16x4x2_t __rv;
  __builtin_neon_vzipv4hi ((int16x4_t *) &__rv.val[0], (int16x4_t) __a, (int16x4_t) __b);
  return __rv;
}

__extension__ static __inline int8x16x2_t __attribute__ ((__always_inline__))
vzipq_s8 (int8x16_t __a, int8x16_t __b)
{
  int8x16x2_t __rv;
  __builtin_neon_vzipv16qi (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline int16x8x2_t __attribute__ ((__always_inline__))
vzipq_s16 (int16x8_t __a, int16x8_t __b)
{
  int16x8x2_t __rv;
  __builtin_neon_vzipv8hi (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline int32x4x2_t __attribute__ ((__always_inline__))
vzipq_s32 (int32x4_t __a, int32x4_t __b)
{
  int32x4x2_t __rv;
  __builtin_neon_vzipv4si (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline float32x4x2_t __attribute__ ((__always_inline__))
vzipq_f32 (float32x4_t __a, float32x4_t __b)
{
  float32x4x2_t __rv;
  __builtin_neon_vzipv4sf (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline uint8x16x2_t __attribute__ ((__always_inline__))
vzipq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  uint8x16x2_t __rv;
  __builtin_neon_vzipv16qi ((int8x16_t *) &__rv.val[0], (int8x16_t) __a, (int8x16_t) __b);
  return __rv;
}

__extension__ static __inline uint16x8x2_t __attribute__ ((__always_inline__))
vzipq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  uint16x8x2_t __rv;
  __builtin_neon_vzipv8hi ((int16x8_t *) &__rv.val[0], (int16x8_t) __a, (int16x8_t) __b);
  return __rv;
}

__extension__ static __inline uint32x4x2_t __attribute__ ((__always_inline__))
vzipq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  uint32x4x2_t __rv;
  __builtin_neon_vzipv4si ((int32x4_t *) &__rv.val[0], (int32x4_t) __a, (int32x4_t) __b);
  return __rv;
}

__extension__ static __inline poly8x16x2_t __attribute__ ((__always_inline__))
vzipq_p8 (poly8x16_t __a, poly8x16_t __b)
{
  poly8x16x2_t __rv;
  __builtin_neon_vzipv16qi ((int8x16_t *) &__rv.val[0], (int8x16_t) __a, (int8x16_t) __b);
  return __rv;
}

__extension__ static __inline poly16x8x2_t __attribute__ ((__always_inline__))
vzipq_p16 (poly16x8_t __a, poly16x8_t __b)
{
  poly16x8x2_t __rv;
  __builtin_neon_vzipv8hi ((int16x8_t *) &__rv.val[0], (int16x8_t) __a, (int16x8_t) __b);
  return __rv;
}

__extension__ static __inline int8x8x2_t __attribute__ ((__always_inline__))
vuzp_s8 (int8x8_t __a, int8x8_t __b)
{
  int8x8x2_t __rv;
  __builtin_neon_vuzpv8qi (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline int16x4x2_t __attribute__ ((__always_inline__))
vuzp_s16 (int16x4_t __a, int16x4_t __b)
{
  int16x4x2_t __rv;
  __builtin_neon_vuzpv4hi (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline int32x2x2_t __attribute__ ((__always_inline__))
vuzp_s32 (int32x2_t __a, int32x2_t __b)
{
  int32x2x2_t __rv;
  __builtin_neon_vuzpv2si (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline float32x2x2_t __attribute__ ((__always_inline__))
vuzp_f32 (float32x2_t __a, float32x2_t __b)
{
  float32x2x2_t __rv;
  __builtin_neon_vuzpv2sf (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline uint8x8x2_t __attribute__ ((__always_inline__))
vuzp_u8 (uint8x8_t __a, uint8x8_t __b)
{
  uint8x8x2_t __rv;
  __builtin_neon_vuzpv8qi ((int8x8_t *) &__rv.val[0], (int8x8_t) __a, (int8x8_t) __b);
  return __rv;
}

__extension__ static __inline uint16x4x2_t __attribute__ ((__always_inline__))
vuzp_u16 (uint16x4_t __a, uint16x4_t __b)
{
  uint16x4x2_t __rv;
  __builtin_neon_vuzpv4hi ((int16x4_t *) &__rv.val[0], (int16x4_t) __a, (int16x4_t) __b);
  return __rv;
}

__extension__ static __inline uint32x2x2_t __attribute__ ((__always_inline__))
vuzp_u32 (uint32x2_t __a, uint32x2_t __b)
{
  uint32x2x2_t __rv;
  __builtin_neon_vuzpv2si ((int32x2_t *) &__rv.val[0], (int32x2_t) __a, (int32x2_t) __b);
  return __rv;
}

__extension__ static __inline poly8x8x2_t __attribute__ ((__always_inline__))
vuzp_p8 (poly8x8_t __a, poly8x8_t __b)
{
  poly8x8x2_t __rv;
  __builtin_neon_vuzpv8qi ((int8x8_t *) &__rv.val[0], (int8x8_t) __a, (int8x8_t) __b);
  return __rv;
}

__extension__ static __inline poly16x4x2_t __attribute__ ((__always_inline__))
vuzp_p16 (poly16x4_t __a, poly16x4_t __b)
{
  poly16x4x2_t __rv;
  __builtin_neon_vuzpv4hi ((int16x4_t *) &__rv.val[0], (int16x4_t) __a, (int16x4_t) __b);
  return __rv;
}

__extension__ static __inline int8x16x2_t __attribute__ ((__always_inline__))
vuzpq_s8 (int8x16_t __a, int8x16_t __b)
{
  int8x16x2_t __rv;
  __builtin_neon_vuzpv16qi (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline int16x8x2_t __attribute__ ((__always_inline__))
vuzpq_s16 (int16x8_t __a, int16x8_t __b)
{
  int16x8x2_t __rv;
  __builtin_neon_vuzpv8hi (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline int32x4x2_t __attribute__ ((__always_inline__))
vuzpq_s32 (int32x4_t __a, int32x4_t __b)
{
  int32x4x2_t __rv;
  __builtin_neon_vuzpv4si (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline float32x4x2_t __attribute__ ((__always_inline__))
vuzpq_f32 (float32x4_t __a, float32x4_t __b)
{
  float32x4x2_t __rv;
  __builtin_neon_vuzpv4sf (&__rv.val[0], __a, __b);
  return __rv;
}

__extension__ static __inline uint8x16x2_t __attribute__ ((__always_inline__))
vuzpq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  uint8x16x2_t __rv;
  __builtin_neon_vuzpv16qi ((int8x16_t *) &__rv.val[0], (int8x16_t) __a, (int8x16_t) __b);
  return __rv;
}

__extension__ static __inline uint16x8x2_t __attribute__ ((__always_inline__))
vuzpq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  uint16x8x2_t __rv;
  __builtin_neon_vuzpv8hi ((int16x8_t *) &__rv.val[0], (int16x8_t) __a, (int16x8_t) __b);
  return __rv;
}

__extension__ static __inline uint32x4x2_t __attribute__ ((__always_inline__))
vuzpq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  uint32x4x2_t __rv;
  __builtin_neon_vuzpv4si ((int32x4_t *) &__rv.val[0], (int32x4_t) __a, (int32x4_t) __b);
  return __rv;
}

__extension__ static __inline poly8x16x2_t __attribute__ ((__always_inline__))
vuzpq_p8 (poly8x16_t __a, poly8x16_t __b)
{
  poly8x16x2_t __rv;
  __builtin_neon_vuzpv16qi ((int8x16_t *) &__rv.val[0], (int8x16_t) __a, (int8x16_t) __b);
  return __rv;
}

__extension__ static __inline poly16x8x2_t __attribute__ ((__always_inline__))
vuzpq_p16 (poly16x8_t __a, poly16x8_t __b)
{
  poly16x8x2_t __rv;
  __builtin_neon_vuzpv8hi ((int16x8_t *) &__rv.val[0], (int16x8_t) __a, (int16x8_t) __b);
  return __rv;
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vld1_s8 (const int8_t * __a)
{
  return (int8x8_t)__builtin_neon_vld1v8qi ((const __builtin_neon_qi *) __a);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vld1_s16 (const int16_t * __a)
{
  return (int16x4_t)__builtin_neon_vld1v4hi ((const __builtin_neon_hi *) __a);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vld1_s32 (const int32_t * __a)
{
  return (int32x2_t)__builtin_neon_vld1v2si ((const __builtin_neon_si *) __a);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vld1_s64 (const int64_t * __a)
{
  return (int64x1_t)__builtin_neon_vld1di ((const __builtin_neon_di *) __a);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vld1_f32 (const float32_t * __a)
{
  return (float32x2_t)__builtin_neon_vld1v2sf ((const __builtin_neon_sf *) __a);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vld1_u8 (const uint8_t * __a)
{
  return (uint8x8_t)__builtin_neon_vld1v8qi ((const __builtin_neon_qi *) __a);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vld1_u16 (const uint16_t * __a)
{
  return (uint16x4_t)__builtin_neon_vld1v4hi ((const __builtin_neon_hi *) __a);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vld1_u32 (const uint32_t * __a)
{
  return (uint32x2_t)__builtin_neon_vld1v2si ((const __builtin_neon_si *) __a);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vld1_u64 (const uint64_t * __a)
{
  return (uint64x1_t)__builtin_neon_vld1di ((const __builtin_neon_di *) __a);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vld1_p8 (const poly8_t * __a)
{
  return (poly8x8_t)__builtin_neon_vld1v8qi ((const __builtin_neon_qi *) __a);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vld1_p16 (const poly16_t * __a)
{
  return (poly16x4_t)__builtin_neon_vld1v4hi ((const __builtin_neon_hi *) __a);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vld1q_s8 (const int8_t * __a)
{
  return (int8x16_t)__builtin_neon_vld1v16qi ((const __builtin_neon_qi *) __a);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vld1q_s16 (const int16_t * __a)
{
  return (int16x8_t)__builtin_neon_vld1v8hi ((const __builtin_neon_hi *) __a);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vld1q_s32 (const int32_t * __a)
{
  return (int32x4_t)__builtin_neon_vld1v4si ((const __builtin_neon_si *) __a);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vld1q_s64 (const int64_t * __a)
{
  return (int64x2_t)__builtin_neon_vld1v2di ((const __builtin_neon_di *) __a);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vld1q_f32 (const float32_t * __a)
{
  return (float32x4_t)__builtin_neon_vld1v4sf ((const __builtin_neon_sf *) __a);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vld1q_u8 (const uint8_t * __a)
{
  return (uint8x16_t)__builtin_neon_vld1v16qi ((const __builtin_neon_qi *) __a);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vld1q_u16 (const uint16_t * __a)
{
  return (uint16x8_t)__builtin_neon_vld1v8hi ((const __builtin_neon_hi *) __a);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vld1q_u32 (const uint32_t * __a)
{
  return (uint32x4_t)__builtin_neon_vld1v4si ((const __builtin_neon_si *) __a);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vld1q_u64 (const uint64_t * __a)
{
  return (uint64x2_t)__builtin_neon_vld1v2di ((const __builtin_neon_di *) __a);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vld1q_p8 (const poly8_t * __a)
{
  return (poly8x16_t)__builtin_neon_vld1v16qi ((const __builtin_neon_qi *) __a);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vld1q_p16 (const poly16_t * __a)
{
  return (poly16x8_t)__builtin_neon_vld1v8hi ((const __builtin_neon_hi *) __a);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vld1_lane_s8 (const int8_t * __a, int8x8_t __b, const int __c)
{
  return (int8x8_t)__builtin_neon_vld1_lanev8qi ((const __builtin_neon_qi *) __a, __b, __c);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vld1_lane_s16 (const int16_t * __a, int16x4_t __b, const int __c)
{
  return (int16x4_t)__builtin_neon_vld1_lanev4hi ((const __builtin_neon_hi *) __a, __b, __c);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vld1_lane_s32 (const int32_t * __a, int32x2_t __b, const int __c)
{
  return (int32x2_t)__builtin_neon_vld1_lanev2si ((const __builtin_neon_si *) __a, __b, __c);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vld1_lane_f32 (const float32_t * __a, float32x2_t __b, const int __c)
{
  return (float32x2_t)__builtin_neon_vld1_lanev2sf ((const __builtin_neon_sf *) __a, __b, __c);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vld1_lane_u8 (const uint8_t * __a, uint8x8_t __b, const int __c)
{
  return (uint8x8_t)__builtin_neon_vld1_lanev8qi ((const __builtin_neon_qi *) __a, (int8x8_t) __b, __c);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vld1_lane_u16 (const uint16_t * __a, uint16x4_t __b, const int __c)
{
  return (uint16x4_t)__builtin_neon_vld1_lanev4hi ((const __builtin_neon_hi *) __a, (int16x4_t) __b, __c);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vld1_lane_u32 (const uint32_t * __a, uint32x2_t __b, const int __c)
{
  return (uint32x2_t)__builtin_neon_vld1_lanev2si ((const __builtin_neon_si *) __a, (int32x2_t) __b, __c);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vld1_lane_p8 (const poly8_t * __a, poly8x8_t __b, const int __c)
{
  return (poly8x8_t)__builtin_neon_vld1_lanev8qi ((const __builtin_neon_qi *) __a, (int8x8_t) __b, __c);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vld1_lane_p16 (const poly16_t * __a, poly16x4_t __b, const int __c)
{
  return (poly16x4_t)__builtin_neon_vld1_lanev4hi ((const __builtin_neon_hi *) __a, (int16x4_t) __b, __c);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vld1_lane_s64 (const int64_t * __a, int64x1_t __b, const int __c)
{
  return (int64x1_t)__builtin_neon_vld1_lanedi ((const __builtin_neon_di *) __a, __b, __c);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vld1_lane_u64 (const uint64_t * __a, uint64x1_t __b, const int __c)
{
  return (uint64x1_t)__builtin_neon_vld1_lanedi ((const __builtin_neon_di *) __a, (int64x1_t) __b, __c);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vld1q_lane_s8 (const int8_t * __a, int8x16_t __b, const int __c)
{
  return (int8x16_t)__builtin_neon_vld1_lanev16qi ((const __builtin_neon_qi *) __a, __b, __c);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vld1q_lane_s16 (const int16_t * __a, int16x8_t __b, const int __c)
{
  return (int16x8_t)__builtin_neon_vld1_lanev8hi ((const __builtin_neon_hi *) __a, __b, __c);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vld1q_lane_s32 (const int32_t * __a, int32x4_t __b, const int __c)
{
  return (int32x4_t)__builtin_neon_vld1_lanev4si ((const __builtin_neon_si *) __a, __b, __c);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vld1q_lane_f32 (const float32_t * __a, float32x4_t __b, const int __c)
{
  return (float32x4_t)__builtin_neon_vld1_lanev4sf ((const __builtin_neon_sf *) __a, __b, __c);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vld1q_lane_u8 (const uint8_t * __a, uint8x16_t __b, const int __c)
{
  return (uint8x16_t)__builtin_neon_vld1_lanev16qi ((const __builtin_neon_qi *) __a, (int8x16_t) __b, __c);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vld1q_lane_u16 (const uint16_t * __a, uint16x8_t __b, const int __c)
{
  return (uint16x8_t)__builtin_neon_vld1_lanev8hi ((const __builtin_neon_hi *) __a, (int16x8_t) __b, __c);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vld1q_lane_u32 (const uint32_t * __a, uint32x4_t __b, const int __c)
{
  return (uint32x4_t)__builtin_neon_vld1_lanev4si ((const __builtin_neon_si *) __a, (int32x4_t) __b, __c);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vld1q_lane_p8 (const poly8_t * __a, poly8x16_t __b, const int __c)
{
  return (poly8x16_t)__builtin_neon_vld1_lanev16qi ((const __builtin_neon_qi *) __a, (int8x16_t) __b, __c);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vld1q_lane_p16 (const poly16_t * __a, poly16x8_t __b, const int __c)
{
  return (poly16x8_t)__builtin_neon_vld1_lanev8hi ((const __builtin_neon_hi *) __a, (int16x8_t) __b, __c);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vld1q_lane_s64 (const int64_t * __a, int64x2_t __b, const int __c)
{
  return (int64x2_t)__builtin_neon_vld1_lanev2di ((const __builtin_neon_di *) __a, __b, __c);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vld1q_lane_u64 (const uint64_t * __a, uint64x2_t __b, const int __c)
{
  return (uint64x2_t)__builtin_neon_vld1_lanev2di ((const __builtin_neon_di *) __a, (int64x2_t) __b, __c);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vld1_dup_s8 (const int8_t * __a)
{
  return (int8x8_t)__builtin_neon_vld1_dupv8qi ((const __builtin_neon_qi *) __a);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vld1_dup_s16 (const int16_t * __a)
{
  return (int16x4_t)__builtin_neon_vld1_dupv4hi ((const __builtin_neon_hi *) __a);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vld1_dup_s32 (const int32_t * __a)
{
  return (int32x2_t)__builtin_neon_vld1_dupv2si ((const __builtin_neon_si *) __a);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vld1_dup_f32 (const float32_t * __a)
{
  return (float32x2_t)__builtin_neon_vld1_dupv2sf ((const __builtin_neon_sf *) __a);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vld1_dup_u8 (const uint8_t * __a)
{
  return (uint8x8_t)__builtin_neon_vld1_dupv8qi ((const __builtin_neon_qi *) __a);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vld1_dup_u16 (const uint16_t * __a)
{
  return (uint16x4_t)__builtin_neon_vld1_dupv4hi ((const __builtin_neon_hi *) __a);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vld1_dup_u32 (const uint32_t * __a)
{
  return (uint32x2_t)__builtin_neon_vld1_dupv2si ((const __builtin_neon_si *) __a);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vld1_dup_p8 (const poly8_t * __a)
{
  return (poly8x8_t)__builtin_neon_vld1_dupv8qi ((const __builtin_neon_qi *) __a);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vld1_dup_p16 (const poly16_t * __a)
{
  return (poly16x4_t)__builtin_neon_vld1_dupv4hi ((const __builtin_neon_hi *) __a);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vld1_dup_s64 (const int64_t * __a)
{
  return (int64x1_t)__builtin_neon_vld1_dupdi ((const __builtin_neon_di *) __a);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vld1_dup_u64 (const uint64_t * __a)
{
  return (uint64x1_t)__builtin_neon_vld1_dupdi ((const __builtin_neon_di *) __a);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vld1q_dup_s8 (const int8_t * __a)
{
  return (int8x16_t)__builtin_neon_vld1_dupv16qi ((const __builtin_neon_qi *) __a);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vld1q_dup_s16 (const int16_t * __a)
{
  return (int16x8_t)__builtin_neon_vld1_dupv8hi ((const __builtin_neon_hi *) __a);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vld1q_dup_s32 (const int32_t * __a)
{
  return (int32x4_t)__builtin_neon_vld1_dupv4si ((const __builtin_neon_si *) __a);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vld1q_dup_f32 (const float32_t * __a)
{
  return (float32x4_t)__builtin_neon_vld1_dupv4sf ((const __builtin_neon_sf *) __a);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vld1q_dup_u8 (const uint8_t * __a)
{
  return (uint8x16_t)__builtin_neon_vld1_dupv16qi ((const __builtin_neon_qi *) __a);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vld1q_dup_u16 (const uint16_t * __a)
{
  return (uint16x8_t)__builtin_neon_vld1_dupv8hi ((const __builtin_neon_hi *) __a);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vld1q_dup_u32 (const uint32_t * __a)
{
  return (uint32x4_t)__builtin_neon_vld1_dupv4si ((const __builtin_neon_si *) __a);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vld1q_dup_p8 (const poly8_t * __a)
{
  return (poly8x16_t)__builtin_neon_vld1_dupv16qi ((const __builtin_neon_qi *) __a);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vld1q_dup_p16 (const poly16_t * __a)
{
  return (poly16x8_t)__builtin_neon_vld1_dupv8hi ((const __builtin_neon_hi *) __a);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vld1q_dup_s64 (const int64_t * __a)
{
  return (int64x2_t)__builtin_neon_vld1_dupv2di ((const __builtin_neon_di *) __a);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vld1q_dup_u64 (const uint64_t * __a)
{
  return (uint64x2_t)__builtin_neon_vld1_dupv2di ((const __builtin_neon_di *) __a);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_s8 (int8_t * __a, int8x8_t __b)
{
  __builtin_neon_vst1v8qi ((__builtin_neon_qi *) __a, __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_s16 (int16_t * __a, int16x4_t __b)
{
  __builtin_neon_vst1v4hi ((__builtin_neon_hi *) __a, __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_s32 (int32_t * __a, int32x2_t __b)
{
  __builtin_neon_vst1v2si ((__builtin_neon_si *) __a, __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_s64 (int64_t * __a, int64x1_t __b)
{
  __builtin_neon_vst1di ((__builtin_neon_di *) __a, __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_f32 (float32_t * __a, float32x2_t __b)
{
  __builtin_neon_vst1v2sf ((__builtin_neon_sf *) __a, __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_u8 (uint8_t * __a, uint8x8_t __b)
{
  __builtin_neon_vst1v8qi ((__builtin_neon_qi *) __a, (int8x8_t) __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_u16 (uint16_t * __a, uint16x4_t __b)
{
  __builtin_neon_vst1v4hi ((__builtin_neon_hi *) __a, (int16x4_t) __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_u32 (uint32_t * __a, uint32x2_t __b)
{
  __builtin_neon_vst1v2si ((__builtin_neon_si *) __a, (int32x2_t) __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_u64 (uint64_t * __a, uint64x1_t __b)
{
  __builtin_neon_vst1di ((__builtin_neon_di *) __a, (int64x1_t) __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_p8 (poly8_t * __a, poly8x8_t __b)
{
  __builtin_neon_vst1v8qi ((__builtin_neon_qi *) __a, (int8x8_t) __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_p16 (poly16_t * __a, poly16x4_t __b)
{
  __builtin_neon_vst1v4hi ((__builtin_neon_hi *) __a, (int16x4_t) __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_s8 (int8_t * __a, int8x16_t __b)
{
  __builtin_neon_vst1v16qi ((__builtin_neon_qi *) __a, __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_s16 (int16_t * __a, int16x8_t __b)
{
  __builtin_neon_vst1v8hi ((__builtin_neon_hi *) __a, __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_s32 (int32_t * __a, int32x4_t __b)
{
  __builtin_neon_vst1v4si ((__builtin_neon_si *) __a, __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_s64 (int64_t * __a, int64x2_t __b)
{
  __builtin_neon_vst1v2di ((__builtin_neon_di *) __a, __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_f32 (float32_t * __a, float32x4_t __b)
{
  __builtin_neon_vst1v4sf ((__builtin_neon_sf *) __a, __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_u8 (uint8_t * __a, uint8x16_t __b)
{
  __builtin_neon_vst1v16qi ((__builtin_neon_qi *) __a, (int8x16_t) __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_u16 (uint16_t * __a, uint16x8_t __b)
{
  __builtin_neon_vst1v8hi ((__builtin_neon_hi *) __a, (int16x8_t) __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_u32 (uint32_t * __a, uint32x4_t __b)
{
  __builtin_neon_vst1v4si ((__builtin_neon_si *) __a, (int32x4_t) __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_u64 (uint64_t * __a, uint64x2_t __b)
{
  __builtin_neon_vst1v2di ((__builtin_neon_di *) __a, (int64x2_t) __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_p8 (poly8_t * __a, poly8x16_t __b)
{
  __builtin_neon_vst1v16qi ((__builtin_neon_qi *) __a, (int8x16_t) __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_p16 (poly16_t * __a, poly16x8_t __b)
{
  __builtin_neon_vst1v8hi ((__builtin_neon_hi *) __a, (int16x8_t) __b);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_lane_s8 (int8_t * __a, int8x8_t __b, const int __c)
{
  __builtin_neon_vst1_lanev8qi ((__builtin_neon_qi *) __a, __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_lane_s16 (int16_t * __a, int16x4_t __b, const int __c)
{
  __builtin_neon_vst1_lanev4hi ((__builtin_neon_hi *) __a, __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_lane_s32 (int32_t * __a, int32x2_t __b, const int __c)
{
  __builtin_neon_vst1_lanev2si ((__builtin_neon_si *) __a, __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_lane_f32 (float32_t * __a, float32x2_t __b, const int __c)
{
  __builtin_neon_vst1_lanev2sf ((__builtin_neon_sf *) __a, __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_lane_u8 (uint8_t * __a, uint8x8_t __b, const int __c)
{
  __builtin_neon_vst1_lanev8qi ((__builtin_neon_qi *) __a, (int8x8_t) __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_lane_u16 (uint16_t * __a, uint16x4_t __b, const int __c)
{
  __builtin_neon_vst1_lanev4hi ((__builtin_neon_hi *) __a, (int16x4_t) __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_lane_u32 (uint32_t * __a, uint32x2_t __b, const int __c)
{
  __builtin_neon_vst1_lanev2si ((__builtin_neon_si *) __a, (int32x2_t) __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_lane_p8 (poly8_t * __a, poly8x8_t __b, const int __c)
{
  __builtin_neon_vst1_lanev8qi ((__builtin_neon_qi *) __a, (int8x8_t) __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_lane_p16 (poly16_t * __a, poly16x4_t __b, const int __c)
{
  __builtin_neon_vst1_lanev4hi ((__builtin_neon_hi *) __a, (int16x4_t) __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_lane_s64 (int64_t * __a, int64x1_t __b, const int __c)
{
  __builtin_neon_vst1_lanedi ((__builtin_neon_di *) __a, __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1_lane_u64 (uint64_t * __a, uint64x1_t __b, const int __c)
{
  __builtin_neon_vst1_lanedi ((__builtin_neon_di *) __a, (int64x1_t) __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_lane_s8 (int8_t * __a, int8x16_t __b, const int __c)
{
  __builtin_neon_vst1_lanev16qi ((__builtin_neon_qi *) __a, __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_lane_s16 (int16_t * __a, int16x8_t __b, const int __c)
{
  __builtin_neon_vst1_lanev8hi ((__builtin_neon_hi *) __a, __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_lane_s32 (int32_t * __a, int32x4_t __b, const int __c)
{
  __builtin_neon_vst1_lanev4si ((__builtin_neon_si *) __a, __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_lane_f32 (float32_t * __a, float32x4_t __b, const int __c)
{
  __builtin_neon_vst1_lanev4sf ((__builtin_neon_sf *) __a, __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_lane_u8 (uint8_t * __a, uint8x16_t __b, const int __c)
{
  __builtin_neon_vst1_lanev16qi ((__builtin_neon_qi *) __a, (int8x16_t) __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_lane_u16 (uint16_t * __a, uint16x8_t __b, const int __c)
{
  __builtin_neon_vst1_lanev8hi ((__builtin_neon_hi *) __a, (int16x8_t) __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_lane_u32 (uint32_t * __a, uint32x4_t __b, const int __c)
{
  __builtin_neon_vst1_lanev4si ((__builtin_neon_si *) __a, (int32x4_t) __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_lane_p8 (poly8_t * __a, poly8x16_t __b, const int __c)
{
  __builtin_neon_vst1_lanev16qi ((__builtin_neon_qi *) __a, (int8x16_t) __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_lane_p16 (poly16_t * __a, poly16x8_t __b, const int __c)
{
  __builtin_neon_vst1_lanev8hi ((__builtin_neon_hi *) __a, (int16x8_t) __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_lane_s64 (int64_t * __a, int64x2_t __b, const int __c)
{
  __builtin_neon_vst1_lanev2di ((__builtin_neon_di *) __a, __b, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst1q_lane_u64 (uint64_t * __a, uint64x2_t __b, const int __c)
{
  __builtin_neon_vst1_lanev2di ((__builtin_neon_di *) __a, (int64x2_t) __b, __c);
}

__extension__ static __inline int8x8x2_t __attribute__ ((__always_inline__))
vld2_s8 (const int8_t * __a)
{
  union { int8x8x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2v8qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline int16x4x2_t __attribute__ ((__always_inline__))
vld2_s16 (const int16_t * __a)
{
  union { int16x4x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2v4hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline int32x2x2_t __attribute__ ((__always_inline__))
vld2_s32 (const int32_t * __a)
{
  union { int32x2x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2v2si ((const __builtin_neon_si *) __a);
  return __rv.__i;
}

__extension__ static __inline float32x2x2_t __attribute__ ((__always_inline__))
vld2_f32 (const float32_t * __a)
{
  union { float32x2x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2v2sf ((const __builtin_neon_sf *) __a);
  return __rv.__i;
}

__extension__ static __inline uint8x8x2_t __attribute__ ((__always_inline__))
vld2_u8 (const uint8_t * __a)
{
  union { uint8x8x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2v8qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline uint16x4x2_t __attribute__ ((__always_inline__))
vld2_u16 (const uint16_t * __a)
{
  union { uint16x4x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2v4hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline uint32x2x2_t __attribute__ ((__always_inline__))
vld2_u32 (const uint32_t * __a)
{
  union { uint32x2x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2v2si ((const __builtin_neon_si *) __a);
  return __rv.__i;
}

__extension__ static __inline poly8x8x2_t __attribute__ ((__always_inline__))
vld2_p8 (const poly8_t * __a)
{
  union { poly8x8x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2v8qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline poly16x4x2_t __attribute__ ((__always_inline__))
vld2_p16 (const poly16_t * __a)
{
  union { poly16x4x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2v4hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline int64x1x2_t __attribute__ ((__always_inline__))
vld2_s64 (const int64_t * __a)
{
  union { int64x1x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2di ((const __builtin_neon_di *) __a);
  return __rv.__i;
}

__extension__ static __inline uint64x1x2_t __attribute__ ((__always_inline__))
vld2_u64 (const uint64_t * __a)
{
  union { uint64x1x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2di ((const __builtin_neon_di *) __a);
  return __rv.__i;
}

__extension__ static __inline int8x16x2_t __attribute__ ((__always_inline__))
vld2q_s8 (const int8_t * __a)
{
  union { int8x16x2_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld2v16qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline int16x8x2_t __attribute__ ((__always_inline__))
vld2q_s16 (const int16_t * __a)
{
  union { int16x8x2_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld2v8hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline int32x4x2_t __attribute__ ((__always_inline__))
vld2q_s32 (const int32_t * __a)
{
  union { int32x4x2_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld2v4si ((const __builtin_neon_si *) __a);
  return __rv.__i;
}

__extension__ static __inline float32x4x2_t __attribute__ ((__always_inline__))
vld2q_f32 (const float32_t * __a)
{
  union { float32x4x2_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld2v4sf ((const __builtin_neon_sf *) __a);
  return __rv.__i;
}

__extension__ static __inline uint8x16x2_t __attribute__ ((__always_inline__))
vld2q_u8 (const uint8_t * __a)
{
  union { uint8x16x2_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld2v16qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline uint16x8x2_t __attribute__ ((__always_inline__))
vld2q_u16 (const uint16_t * __a)
{
  union { uint16x8x2_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld2v8hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline uint32x4x2_t __attribute__ ((__always_inline__))
vld2q_u32 (const uint32_t * __a)
{
  union { uint32x4x2_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld2v4si ((const __builtin_neon_si *) __a);
  return __rv.__i;
}

__extension__ static __inline poly8x16x2_t __attribute__ ((__always_inline__))
vld2q_p8 (const poly8_t * __a)
{
  union { poly8x16x2_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld2v16qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline poly16x8x2_t __attribute__ ((__always_inline__))
vld2q_p16 (const poly16_t * __a)
{
  union { poly16x8x2_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld2v8hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline int8x8x2_t __attribute__ ((__always_inline__))
vld2_lane_s8 (const int8_t * __a, int8x8x2_t __b, const int __c)
{
  union { int8x8x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  union { int8x8x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2_lanev8qi ((const __builtin_neon_qi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline int16x4x2_t __attribute__ ((__always_inline__))
vld2_lane_s16 (const int16_t * __a, int16x4x2_t __b, const int __c)
{
  union { int16x4x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  union { int16x4x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2_lanev4hi ((const __builtin_neon_hi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline int32x2x2_t __attribute__ ((__always_inline__))
vld2_lane_s32 (const int32_t * __a, int32x2x2_t __b, const int __c)
{
  union { int32x2x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  union { int32x2x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2_lanev2si ((const __builtin_neon_si *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline float32x2x2_t __attribute__ ((__always_inline__))
vld2_lane_f32 (const float32_t * __a, float32x2x2_t __b, const int __c)
{
  union { float32x2x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  union { float32x2x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2_lanev2sf ((const __builtin_neon_sf *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline uint8x8x2_t __attribute__ ((__always_inline__))
vld2_lane_u8 (const uint8_t * __a, uint8x8x2_t __b, const int __c)
{
  union { uint8x8x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  union { uint8x8x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2_lanev8qi ((const __builtin_neon_qi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline uint16x4x2_t __attribute__ ((__always_inline__))
vld2_lane_u16 (const uint16_t * __a, uint16x4x2_t __b, const int __c)
{
  union { uint16x4x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  union { uint16x4x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2_lanev4hi ((const __builtin_neon_hi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline uint32x2x2_t __attribute__ ((__always_inline__))
vld2_lane_u32 (const uint32_t * __a, uint32x2x2_t __b, const int __c)
{
  union { uint32x2x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  union { uint32x2x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2_lanev2si ((const __builtin_neon_si *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline poly8x8x2_t __attribute__ ((__always_inline__))
vld2_lane_p8 (const poly8_t * __a, poly8x8x2_t __b, const int __c)
{
  union { poly8x8x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  union { poly8x8x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2_lanev8qi ((const __builtin_neon_qi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline poly16x4x2_t __attribute__ ((__always_inline__))
vld2_lane_p16 (const poly16_t * __a, poly16x4x2_t __b, const int __c)
{
  union { poly16x4x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  union { poly16x4x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2_lanev4hi ((const __builtin_neon_hi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline int16x8x2_t __attribute__ ((__always_inline__))
vld2q_lane_s16 (const int16_t * __a, int16x8x2_t __b, const int __c)
{
  union { int16x8x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  union { int16x8x2_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld2_lanev8hi ((const __builtin_neon_hi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline int32x4x2_t __attribute__ ((__always_inline__))
vld2q_lane_s32 (const int32_t * __a, int32x4x2_t __b, const int __c)
{
  union { int32x4x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  union { int32x4x2_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld2_lanev4si ((const __builtin_neon_si *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline float32x4x2_t __attribute__ ((__always_inline__))
vld2q_lane_f32 (const float32_t * __a, float32x4x2_t __b, const int __c)
{
  union { float32x4x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  union { float32x4x2_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld2_lanev4sf ((const __builtin_neon_sf *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline uint16x8x2_t __attribute__ ((__always_inline__))
vld2q_lane_u16 (const uint16_t * __a, uint16x8x2_t __b, const int __c)
{
  union { uint16x8x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  union { uint16x8x2_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld2_lanev8hi ((const __builtin_neon_hi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline uint32x4x2_t __attribute__ ((__always_inline__))
vld2q_lane_u32 (const uint32_t * __a, uint32x4x2_t __b, const int __c)
{
  union { uint32x4x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  union { uint32x4x2_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld2_lanev4si ((const __builtin_neon_si *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline poly16x8x2_t __attribute__ ((__always_inline__))
vld2q_lane_p16 (const poly16_t * __a, poly16x8x2_t __b, const int __c)
{
  union { poly16x8x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  union { poly16x8x2_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld2_lanev8hi ((const __builtin_neon_hi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline int8x8x2_t __attribute__ ((__always_inline__))
vld2_dup_s8 (const int8_t * __a)
{
  union { int8x8x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2_dupv8qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline int16x4x2_t __attribute__ ((__always_inline__))
vld2_dup_s16 (const int16_t * __a)
{
  union { int16x4x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2_dupv4hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline int32x2x2_t __attribute__ ((__always_inline__))
vld2_dup_s32 (const int32_t * __a)
{
  union { int32x2x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2_dupv2si ((const __builtin_neon_si *) __a);
  return __rv.__i;
}

__extension__ static __inline float32x2x2_t __attribute__ ((__always_inline__))
vld2_dup_f32 (const float32_t * __a)
{
  union { float32x2x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2_dupv2sf ((const __builtin_neon_sf *) __a);
  return __rv.__i;
}

__extension__ static __inline uint8x8x2_t __attribute__ ((__always_inline__))
vld2_dup_u8 (const uint8_t * __a)
{
  union { uint8x8x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2_dupv8qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline uint16x4x2_t __attribute__ ((__always_inline__))
vld2_dup_u16 (const uint16_t * __a)
{
  union { uint16x4x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2_dupv4hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline uint32x2x2_t __attribute__ ((__always_inline__))
vld2_dup_u32 (const uint32_t * __a)
{
  union { uint32x2x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2_dupv2si ((const __builtin_neon_si *) __a);
  return __rv.__i;
}

__extension__ static __inline poly8x8x2_t __attribute__ ((__always_inline__))
vld2_dup_p8 (const poly8_t * __a)
{
  union { poly8x8x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2_dupv8qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline poly16x4x2_t __attribute__ ((__always_inline__))
vld2_dup_p16 (const poly16_t * __a)
{
  union { poly16x4x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2_dupv4hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline int64x1x2_t __attribute__ ((__always_inline__))
vld2_dup_s64 (const int64_t * __a)
{
  union { int64x1x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2_dupdi ((const __builtin_neon_di *) __a);
  return __rv.__i;
}

__extension__ static __inline uint64x1x2_t __attribute__ ((__always_inline__))
vld2_dup_u64 (const uint64_t * __a)
{
  union { uint64x1x2_t __i; __builtin_neon_ti __o; } __rv;
  __rv.__o = __builtin_neon_vld2_dupdi ((const __builtin_neon_di *) __a);
  return __rv.__i;
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2_s8 (int8_t * __a, int8x8x2_t __b)
{
  union { int8x8x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  __builtin_neon_vst2v8qi ((__builtin_neon_qi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2_s16 (int16_t * __a, int16x4x2_t __b)
{
  union { int16x4x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  __builtin_neon_vst2v4hi ((__builtin_neon_hi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2_s32 (int32_t * __a, int32x2x2_t __b)
{
  union { int32x2x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  __builtin_neon_vst2v2si ((__builtin_neon_si *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2_f32 (float32_t * __a, float32x2x2_t __b)
{
  union { float32x2x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  __builtin_neon_vst2v2sf ((__builtin_neon_sf *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2_u8 (uint8_t * __a, uint8x8x2_t __b)
{
  union { uint8x8x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  __builtin_neon_vst2v8qi ((__builtin_neon_qi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2_u16 (uint16_t * __a, uint16x4x2_t __b)
{
  union { uint16x4x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  __builtin_neon_vst2v4hi ((__builtin_neon_hi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2_u32 (uint32_t * __a, uint32x2x2_t __b)
{
  union { uint32x2x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  __builtin_neon_vst2v2si ((__builtin_neon_si *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2_p8 (poly8_t * __a, poly8x8x2_t __b)
{
  union { poly8x8x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  __builtin_neon_vst2v8qi ((__builtin_neon_qi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2_p16 (poly16_t * __a, poly16x4x2_t __b)
{
  union { poly16x4x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  __builtin_neon_vst2v4hi ((__builtin_neon_hi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2_s64 (int64_t * __a, int64x1x2_t __b)
{
  union { int64x1x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  __builtin_neon_vst2di ((__builtin_neon_di *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2_u64 (uint64_t * __a, uint64x1x2_t __b)
{
  union { uint64x1x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  __builtin_neon_vst2di ((__builtin_neon_di *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2q_s8 (int8_t * __a, int8x16x2_t __b)
{
  union { int8x16x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst2v16qi ((__builtin_neon_qi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2q_s16 (int16_t * __a, int16x8x2_t __b)
{
  union { int16x8x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst2v8hi ((__builtin_neon_hi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2q_s32 (int32_t * __a, int32x4x2_t __b)
{
  union { int32x4x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst2v4si ((__builtin_neon_si *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2q_f32 (float32_t * __a, float32x4x2_t __b)
{
  union { float32x4x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst2v4sf ((__builtin_neon_sf *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2q_u8 (uint8_t * __a, uint8x16x2_t __b)
{
  union { uint8x16x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst2v16qi ((__builtin_neon_qi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2q_u16 (uint16_t * __a, uint16x8x2_t __b)
{
  union { uint16x8x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst2v8hi ((__builtin_neon_hi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2q_u32 (uint32_t * __a, uint32x4x2_t __b)
{
  union { uint32x4x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst2v4si ((__builtin_neon_si *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2q_p8 (poly8_t * __a, poly8x16x2_t __b)
{
  union { poly8x16x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst2v16qi ((__builtin_neon_qi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2q_p16 (poly16_t * __a, poly16x8x2_t __b)
{
  union { poly16x8x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst2v8hi ((__builtin_neon_hi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2_lane_s8 (int8_t * __a, int8x8x2_t __b, const int __c)
{
  union { int8x8x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  __builtin_neon_vst2_lanev8qi ((__builtin_neon_qi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2_lane_s16 (int16_t * __a, int16x4x2_t __b, const int __c)
{
  union { int16x4x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  __builtin_neon_vst2_lanev4hi ((__builtin_neon_hi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2_lane_s32 (int32_t * __a, int32x2x2_t __b, const int __c)
{
  union { int32x2x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  __builtin_neon_vst2_lanev2si ((__builtin_neon_si *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2_lane_f32 (float32_t * __a, float32x2x2_t __b, const int __c)
{
  union { float32x2x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  __builtin_neon_vst2_lanev2sf ((__builtin_neon_sf *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2_lane_u8 (uint8_t * __a, uint8x8x2_t __b, const int __c)
{
  union { uint8x8x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  __builtin_neon_vst2_lanev8qi ((__builtin_neon_qi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2_lane_u16 (uint16_t * __a, uint16x4x2_t __b, const int __c)
{
  union { uint16x4x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  __builtin_neon_vst2_lanev4hi ((__builtin_neon_hi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2_lane_u32 (uint32_t * __a, uint32x2x2_t __b, const int __c)
{
  union { uint32x2x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  __builtin_neon_vst2_lanev2si ((__builtin_neon_si *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2_lane_p8 (poly8_t * __a, poly8x8x2_t __b, const int __c)
{
  union { poly8x8x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  __builtin_neon_vst2_lanev8qi ((__builtin_neon_qi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2_lane_p16 (poly16_t * __a, poly16x4x2_t __b, const int __c)
{
  union { poly16x4x2_t __i; __builtin_neon_ti __o; } __bu = { __b };
  __builtin_neon_vst2_lanev4hi ((__builtin_neon_hi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2q_lane_s16 (int16_t * __a, int16x8x2_t __b, const int __c)
{
  union { int16x8x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst2_lanev8hi ((__builtin_neon_hi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2q_lane_s32 (int32_t * __a, int32x4x2_t __b, const int __c)
{
  union { int32x4x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst2_lanev4si ((__builtin_neon_si *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2q_lane_f32 (float32_t * __a, float32x4x2_t __b, const int __c)
{
  union { float32x4x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst2_lanev4sf ((__builtin_neon_sf *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2q_lane_u16 (uint16_t * __a, uint16x8x2_t __b, const int __c)
{
  union { uint16x8x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst2_lanev8hi ((__builtin_neon_hi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2q_lane_u32 (uint32_t * __a, uint32x4x2_t __b, const int __c)
{
  union { uint32x4x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst2_lanev4si ((__builtin_neon_si *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst2q_lane_p16 (poly16_t * __a, poly16x8x2_t __b, const int __c)
{
  union { poly16x8x2_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst2_lanev8hi ((__builtin_neon_hi *) __a, __bu.__o, __c);
}

__extension__ static __inline int8x8x3_t __attribute__ ((__always_inline__))
vld3_s8 (const int8_t * __a)
{
  union { int8x8x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3v8qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline int16x4x3_t __attribute__ ((__always_inline__))
vld3_s16 (const int16_t * __a)
{
  union { int16x4x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3v4hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline int32x2x3_t __attribute__ ((__always_inline__))
vld3_s32 (const int32_t * __a)
{
  union { int32x2x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3v2si ((const __builtin_neon_si *) __a);
  return __rv.__i;
}

__extension__ static __inline float32x2x3_t __attribute__ ((__always_inline__))
vld3_f32 (const float32_t * __a)
{
  union { float32x2x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3v2sf ((const __builtin_neon_sf *) __a);
  return __rv.__i;
}

__extension__ static __inline uint8x8x3_t __attribute__ ((__always_inline__))
vld3_u8 (const uint8_t * __a)
{
  union { uint8x8x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3v8qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline uint16x4x3_t __attribute__ ((__always_inline__))
vld3_u16 (const uint16_t * __a)
{
  union { uint16x4x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3v4hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline uint32x2x3_t __attribute__ ((__always_inline__))
vld3_u32 (const uint32_t * __a)
{
  union { uint32x2x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3v2si ((const __builtin_neon_si *) __a);
  return __rv.__i;
}

__extension__ static __inline poly8x8x3_t __attribute__ ((__always_inline__))
vld3_p8 (const poly8_t * __a)
{
  union { poly8x8x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3v8qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline poly16x4x3_t __attribute__ ((__always_inline__))
vld3_p16 (const poly16_t * __a)
{
  union { poly16x4x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3v4hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline int64x1x3_t __attribute__ ((__always_inline__))
vld3_s64 (const int64_t * __a)
{
  union { int64x1x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3di ((const __builtin_neon_di *) __a);
  return __rv.__i;
}

__extension__ static __inline uint64x1x3_t __attribute__ ((__always_inline__))
vld3_u64 (const uint64_t * __a)
{
  union { uint64x1x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3di ((const __builtin_neon_di *) __a);
  return __rv.__i;
}

__extension__ static __inline int8x16x3_t __attribute__ ((__always_inline__))
vld3q_s8 (const int8_t * __a)
{
  union { int8x16x3_t __i; __builtin_neon_ci __o; } __rv;
  __rv.__o = __builtin_neon_vld3v16qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline int16x8x3_t __attribute__ ((__always_inline__))
vld3q_s16 (const int16_t * __a)
{
  union { int16x8x3_t __i; __builtin_neon_ci __o; } __rv;
  __rv.__o = __builtin_neon_vld3v8hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline int32x4x3_t __attribute__ ((__always_inline__))
vld3q_s32 (const int32_t * __a)
{
  union { int32x4x3_t __i; __builtin_neon_ci __o; } __rv;
  __rv.__o = __builtin_neon_vld3v4si ((const __builtin_neon_si *) __a);
  return __rv.__i;
}

__extension__ static __inline float32x4x3_t __attribute__ ((__always_inline__))
vld3q_f32 (const float32_t * __a)
{
  union { float32x4x3_t __i; __builtin_neon_ci __o; } __rv;
  __rv.__o = __builtin_neon_vld3v4sf ((const __builtin_neon_sf *) __a);
  return __rv.__i;
}

__extension__ static __inline uint8x16x3_t __attribute__ ((__always_inline__))
vld3q_u8 (const uint8_t * __a)
{
  union { uint8x16x3_t __i; __builtin_neon_ci __o; } __rv;
  __rv.__o = __builtin_neon_vld3v16qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline uint16x8x3_t __attribute__ ((__always_inline__))
vld3q_u16 (const uint16_t * __a)
{
  union { uint16x8x3_t __i; __builtin_neon_ci __o; } __rv;
  __rv.__o = __builtin_neon_vld3v8hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline uint32x4x3_t __attribute__ ((__always_inline__))
vld3q_u32 (const uint32_t * __a)
{
  union { uint32x4x3_t __i; __builtin_neon_ci __o; } __rv;
  __rv.__o = __builtin_neon_vld3v4si ((const __builtin_neon_si *) __a);
  return __rv.__i;
}

__extension__ static __inline poly8x16x3_t __attribute__ ((__always_inline__))
vld3q_p8 (const poly8_t * __a)
{
  union { poly8x16x3_t __i; __builtin_neon_ci __o; } __rv;
  __rv.__o = __builtin_neon_vld3v16qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline poly16x8x3_t __attribute__ ((__always_inline__))
vld3q_p16 (const poly16_t * __a)
{
  union { poly16x8x3_t __i; __builtin_neon_ci __o; } __rv;
  __rv.__o = __builtin_neon_vld3v8hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline int8x8x3_t __attribute__ ((__always_inline__))
vld3_lane_s8 (const int8_t * __a, int8x8x3_t __b, const int __c)
{
  union { int8x8x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  union { int8x8x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3_lanev8qi ((const __builtin_neon_qi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline int16x4x3_t __attribute__ ((__always_inline__))
vld3_lane_s16 (const int16_t * __a, int16x4x3_t __b, const int __c)
{
  union { int16x4x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  union { int16x4x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3_lanev4hi ((const __builtin_neon_hi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline int32x2x3_t __attribute__ ((__always_inline__))
vld3_lane_s32 (const int32_t * __a, int32x2x3_t __b, const int __c)
{
  union { int32x2x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  union { int32x2x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3_lanev2si ((const __builtin_neon_si *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline float32x2x3_t __attribute__ ((__always_inline__))
vld3_lane_f32 (const float32_t * __a, float32x2x3_t __b, const int __c)
{
  union { float32x2x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  union { float32x2x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3_lanev2sf ((const __builtin_neon_sf *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline uint8x8x3_t __attribute__ ((__always_inline__))
vld3_lane_u8 (const uint8_t * __a, uint8x8x3_t __b, const int __c)
{
  union { uint8x8x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  union { uint8x8x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3_lanev8qi ((const __builtin_neon_qi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline uint16x4x3_t __attribute__ ((__always_inline__))
vld3_lane_u16 (const uint16_t * __a, uint16x4x3_t __b, const int __c)
{
  union { uint16x4x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  union { uint16x4x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3_lanev4hi ((const __builtin_neon_hi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline uint32x2x3_t __attribute__ ((__always_inline__))
vld3_lane_u32 (const uint32_t * __a, uint32x2x3_t __b, const int __c)
{
  union { uint32x2x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  union { uint32x2x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3_lanev2si ((const __builtin_neon_si *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline poly8x8x3_t __attribute__ ((__always_inline__))
vld3_lane_p8 (const poly8_t * __a, poly8x8x3_t __b, const int __c)
{
  union { poly8x8x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  union { poly8x8x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3_lanev8qi ((const __builtin_neon_qi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline poly16x4x3_t __attribute__ ((__always_inline__))
vld3_lane_p16 (const poly16_t * __a, poly16x4x3_t __b, const int __c)
{
  union { poly16x4x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  union { poly16x4x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3_lanev4hi ((const __builtin_neon_hi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline int16x8x3_t __attribute__ ((__always_inline__))
vld3q_lane_s16 (const int16_t * __a, int16x8x3_t __b, const int __c)
{
  union { int16x8x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  union { int16x8x3_t __i; __builtin_neon_ci __o; } __rv;
  __rv.__o = __builtin_neon_vld3_lanev8hi ((const __builtin_neon_hi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline int32x4x3_t __attribute__ ((__always_inline__))
vld3q_lane_s32 (const int32_t * __a, int32x4x3_t __b, const int __c)
{
  union { int32x4x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  union { int32x4x3_t __i; __builtin_neon_ci __o; } __rv;
  __rv.__o = __builtin_neon_vld3_lanev4si ((const __builtin_neon_si *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline float32x4x3_t __attribute__ ((__always_inline__))
vld3q_lane_f32 (const float32_t * __a, float32x4x3_t __b, const int __c)
{
  union { float32x4x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  union { float32x4x3_t __i; __builtin_neon_ci __o; } __rv;
  __rv.__o = __builtin_neon_vld3_lanev4sf ((const __builtin_neon_sf *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline uint16x8x3_t __attribute__ ((__always_inline__))
vld3q_lane_u16 (const uint16_t * __a, uint16x8x3_t __b, const int __c)
{
  union { uint16x8x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  union { uint16x8x3_t __i; __builtin_neon_ci __o; } __rv;
  __rv.__o = __builtin_neon_vld3_lanev8hi ((const __builtin_neon_hi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline uint32x4x3_t __attribute__ ((__always_inline__))
vld3q_lane_u32 (const uint32_t * __a, uint32x4x3_t __b, const int __c)
{
  union { uint32x4x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  union { uint32x4x3_t __i; __builtin_neon_ci __o; } __rv;
  __rv.__o = __builtin_neon_vld3_lanev4si ((const __builtin_neon_si *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline poly16x8x3_t __attribute__ ((__always_inline__))
vld3q_lane_p16 (const poly16_t * __a, poly16x8x3_t __b, const int __c)
{
  union { poly16x8x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  union { poly16x8x3_t __i; __builtin_neon_ci __o; } __rv;
  __rv.__o = __builtin_neon_vld3_lanev8hi ((const __builtin_neon_hi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline int8x8x3_t __attribute__ ((__always_inline__))
vld3_dup_s8 (const int8_t * __a)
{
  union { int8x8x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3_dupv8qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline int16x4x3_t __attribute__ ((__always_inline__))
vld3_dup_s16 (const int16_t * __a)
{
  union { int16x4x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3_dupv4hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline int32x2x3_t __attribute__ ((__always_inline__))
vld3_dup_s32 (const int32_t * __a)
{
  union { int32x2x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3_dupv2si ((const __builtin_neon_si *) __a);
  return __rv.__i;
}

__extension__ static __inline float32x2x3_t __attribute__ ((__always_inline__))
vld3_dup_f32 (const float32_t * __a)
{
  union { float32x2x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3_dupv2sf ((const __builtin_neon_sf *) __a);
  return __rv.__i;
}

__extension__ static __inline uint8x8x3_t __attribute__ ((__always_inline__))
vld3_dup_u8 (const uint8_t * __a)
{
  union { uint8x8x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3_dupv8qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline uint16x4x3_t __attribute__ ((__always_inline__))
vld3_dup_u16 (const uint16_t * __a)
{
  union { uint16x4x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3_dupv4hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline uint32x2x3_t __attribute__ ((__always_inline__))
vld3_dup_u32 (const uint32_t * __a)
{
  union { uint32x2x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3_dupv2si ((const __builtin_neon_si *) __a);
  return __rv.__i;
}

__extension__ static __inline poly8x8x3_t __attribute__ ((__always_inline__))
vld3_dup_p8 (const poly8_t * __a)
{
  union { poly8x8x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3_dupv8qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline poly16x4x3_t __attribute__ ((__always_inline__))
vld3_dup_p16 (const poly16_t * __a)
{
  union { poly16x4x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3_dupv4hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline int64x1x3_t __attribute__ ((__always_inline__))
vld3_dup_s64 (const int64_t * __a)
{
  union { int64x1x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3_dupdi ((const __builtin_neon_di *) __a);
  return __rv.__i;
}

__extension__ static __inline uint64x1x3_t __attribute__ ((__always_inline__))
vld3_dup_u64 (const uint64_t * __a)
{
  union { uint64x1x3_t __i; __builtin_neon_ei __o; } __rv;
  __rv.__o = __builtin_neon_vld3_dupdi ((const __builtin_neon_di *) __a);
  return __rv.__i;
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3_s8 (int8_t * __a, int8x8x3_t __b)
{
  union { int8x8x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  __builtin_neon_vst3v8qi ((__builtin_neon_qi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3_s16 (int16_t * __a, int16x4x3_t __b)
{
  union { int16x4x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  __builtin_neon_vst3v4hi ((__builtin_neon_hi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3_s32 (int32_t * __a, int32x2x3_t __b)
{
  union { int32x2x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  __builtin_neon_vst3v2si ((__builtin_neon_si *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3_f32 (float32_t * __a, float32x2x3_t __b)
{
  union { float32x2x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  __builtin_neon_vst3v2sf ((__builtin_neon_sf *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3_u8 (uint8_t * __a, uint8x8x3_t __b)
{
  union { uint8x8x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  __builtin_neon_vst3v8qi ((__builtin_neon_qi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3_u16 (uint16_t * __a, uint16x4x3_t __b)
{
  union { uint16x4x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  __builtin_neon_vst3v4hi ((__builtin_neon_hi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3_u32 (uint32_t * __a, uint32x2x3_t __b)
{
  union { uint32x2x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  __builtin_neon_vst3v2si ((__builtin_neon_si *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3_p8 (poly8_t * __a, poly8x8x3_t __b)
{
  union { poly8x8x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  __builtin_neon_vst3v8qi ((__builtin_neon_qi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3_p16 (poly16_t * __a, poly16x4x3_t __b)
{
  union { poly16x4x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  __builtin_neon_vst3v4hi ((__builtin_neon_hi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3_s64 (int64_t * __a, int64x1x3_t __b)
{
  union { int64x1x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  __builtin_neon_vst3di ((__builtin_neon_di *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3_u64 (uint64_t * __a, uint64x1x3_t __b)
{
  union { uint64x1x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  __builtin_neon_vst3di ((__builtin_neon_di *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3q_s8 (int8_t * __a, int8x16x3_t __b)
{
  union { int8x16x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  __builtin_neon_vst3v16qi ((__builtin_neon_qi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3q_s16 (int16_t * __a, int16x8x3_t __b)
{
  union { int16x8x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  __builtin_neon_vst3v8hi ((__builtin_neon_hi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3q_s32 (int32_t * __a, int32x4x3_t __b)
{
  union { int32x4x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  __builtin_neon_vst3v4si ((__builtin_neon_si *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3q_f32 (float32_t * __a, float32x4x3_t __b)
{
  union { float32x4x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  __builtin_neon_vst3v4sf ((__builtin_neon_sf *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3q_u8 (uint8_t * __a, uint8x16x3_t __b)
{
  union { uint8x16x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  __builtin_neon_vst3v16qi ((__builtin_neon_qi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3q_u16 (uint16_t * __a, uint16x8x3_t __b)
{
  union { uint16x8x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  __builtin_neon_vst3v8hi ((__builtin_neon_hi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3q_u32 (uint32_t * __a, uint32x4x3_t __b)
{
  union { uint32x4x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  __builtin_neon_vst3v4si ((__builtin_neon_si *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3q_p8 (poly8_t * __a, poly8x16x3_t __b)
{
  union { poly8x16x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  __builtin_neon_vst3v16qi ((__builtin_neon_qi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3q_p16 (poly16_t * __a, poly16x8x3_t __b)
{
  union { poly16x8x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  __builtin_neon_vst3v8hi ((__builtin_neon_hi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3_lane_s8 (int8_t * __a, int8x8x3_t __b, const int __c)
{
  union { int8x8x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  __builtin_neon_vst3_lanev8qi ((__builtin_neon_qi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3_lane_s16 (int16_t * __a, int16x4x3_t __b, const int __c)
{
  union { int16x4x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  __builtin_neon_vst3_lanev4hi ((__builtin_neon_hi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3_lane_s32 (int32_t * __a, int32x2x3_t __b, const int __c)
{
  union { int32x2x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  __builtin_neon_vst3_lanev2si ((__builtin_neon_si *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3_lane_f32 (float32_t * __a, float32x2x3_t __b, const int __c)
{
  union { float32x2x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  __builtin_neon_vst3_lanev2sf ((__builtin_neon_sf *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3_lane_u8 (uint8_t * __a, uint8x8x3_t __b, const int __c)
{
  union { uint8x8x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  __builtin_neon_vst3_lanev8qi ((__builtin_neon_qi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3_lane_u16 (uint16_t * __a, uint16x4x3_t __b, const int __c)
{
  union { uint16x4x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  __builtin_neon_vst3_lanev4hi ((__builtin_neon_hi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3_lane_u32 (uint32_t * __a, uint32x2x3_t __b, const int __c)
{
  union { uint32x2x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  __builtin_neon_vst3_lanev2si ((__builtin_neon_si *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3_lane_p8 (poly8_t * __a, poly8x8x3_t __b, const int __c)
{
  union { poly8x8x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  __builtin_neon_vst3_lanev8qi ((__builtin_neon_qi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3_lane_p16 (poly16_t * __a, poly16x4x3_t __b, const int __c)
{
  union { poly16x4x3_t __i; __builtin_neon_ei __o; } __bu = { __b };
  __builtin_neon_vst3_lanev4hi ((__builtin_neon_hi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3q_lane_s16 (int16_t * __a, int16x8x3_t __b, const int __c)
{
  union { int16x8x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  __builtin_neon_vst3_lanev8hi ((__builtin_neon_hi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3q_lane_s32 (int32_t * __a, int32x4x3_t __b, const int __c)
{
  union { int32x4x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  __builtin_neon_vst3_lanev4si ((__builtin_neon_si *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3q_lane_f32 (float32_t * __a, float32x4x3_t __b, const int __c)
{
  union { float32x4x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  __builtin_neon_vst3_lanev4sf ((__builtin_neon_sf *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3q_lane_u16 (uint16_t * __a, uint16x8x3_t __b, const int __c)
{
  union { uint16x8x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  __builtin_neon_vst3_lanev8hi ((__builtin_neon_hi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3q_lane_u32 (uint32_t * __a, uint32x4x3_t __b, const int __c)
{
  union { uint32x4x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  __builtin_neon_vst3_lanev4si ((__builtin_neon_si *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst3q_lane_p16 (poly16_t * __a, poly16x8x3_t __b, const int __c)
{
  union { poly16x8x3_t __i; __builtin_neon_ci __o; } __bu = { __b };
  __builtin_neon_vst3_lanev8hi ((__builtin_neon_hi *) __a, __bu.__o, __c);
}

__extension__ static __inline int8x8x4_t __attribute__ ((__always_inline__))
vld4_s8 (const int8_t * __a)
{
  union { int8x8x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4v8qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline int16x4x4_t __attribute__ ((__always_inline__))
vld4_s16 (const int16_t * __a)
{
  union { int16x4x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4v4hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline int32x2x4_t __attribute__ ((__always_inline__))
vld4_s32 (const int32_t * __a)
{
  union { int32x2x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4v2si ((const __builtin_neon_si *) __a);
  return __rv.__i;
}

__extension__ static __inline float32x2x4_t __attribute__ ((__always_inline__))
vld4_f32 (const float32_t * __a)
{
  union { float32x2x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4v2sf ((const __builtin_neon_sf *) __a);
  return __rv.__i;
}

__extension__ static __inline uint8x8x4_t __attribute__ ((__always_inline__))
vld4_u8 (const uint8_t * __a)
{
  union { uint8x8x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4v8qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline uint16x4x4_t __attribute__ ((__always_inline__))
vld4_u16 (const uint16_t * __a)
{
  union { uint16x4x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4v4hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline uint32x2x4_t __attribute__ ((__always_inline__))
vld4_u32 (const uint32_t * __a)
{
  union { uint32x2x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4v2si ((const __builtin_neon_si *) __a);
  return __rv.__i;
}

__extension__ static __inline poly8x8x4_t __attribute__ ((__always_inline__))
vld4_p8 (const poly8_t * __a)
{
  union { poly8x8x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4v8qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline poly16x4x4_t __attribute__ ((__always_inline__))
vld4_p16 (const poly16_t * __a)
{
  union { poly16x4x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4v4hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline int64x1x4_t __attribute__ ((__always_inline__))
vld4_s64 (const int64_t * __a)
{
  union { int64x1x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4di ((const __builtin_neon_di *) __a);
  return __rv.__i;
}

__extension__ static __inline uint64x1x4_t __attribute__ ((__always_inline__))
vld4_u64 (const uint64_t * __a)
{
  union { uint64x1x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4di ((const __builtin_neon_di *) __a);
  return __rv.__i;
}

__extension__ static __inline int8x16x4_t __attribute__ ((__always_inline__))
vld4q_s8 (const int8_t * __a)
{
  union { int8x16x4_t __i; __builtin_neon_xi __o; } __rv;
  __rv.__o = __builtin_neon_vld4v16qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline int16x8x4_t __attribute__ ((__always_inline__))
vld4q_s16 (const int16_t * __a)
{
  union { int16x8x4_t __i; __builtin_neon_xi __o; } __rv;
  __rv.__o = __builtin_neon_vld4v8hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline int32x4x4_t __attribute__ ((__always_inline__))
vld4q_s32 (const int32_t * __a)
{
  union { int32x4x4_t __i; __builtin_neon_xi __o; } __rv;
  __rv.__o = __builtin_neon_vld4v4si ((const __builtin_neon_si *) __a);
  return __rv.__i;
}

__extension__ static __inline float32x4x4_t __attribute__ ((__always_inline__))
vld4q_f32 (const float32_t * __a)
{
  union { float32x4x4_t __i; __builtin_neon_xi __o; } __rv;
  __rv.__o = __builtin_neon_vld4v4sf ((const __builtin_neon_sf *) __a);
  return __rv.__i;
}

__extension__ static __inline uint8x16x4_t __attribute__ ((__always_inline__))
vld4q_u8 (const uint8_t * __a)
{
  union { uint8x16x4_t __i; __builtin_neon_xi __o; } __rv;
  __rv.__o = __builtin_neon_vld4v16qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline uint16x8x4_t __attribute__ ((__always_inline__))
vld4q_u16 (const uint16_t * __a)
{
  union { uint16x8x4_t __i; __builtin_neon_xi __o; } __rv;
  __rv.__o = __builtin_neon_vld4v8hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline uint32x4x4_t __attribute__ ((__always_inline__))
vld4q_u32 (const uint32_t * __a)
{
  union { uint32x4x4_t __i; __builtin_neon_xi __o; } __rv;
  __rv.__o = __builtin_neon_vld4v4si ((const __builtin_neon_si *) __a);
  return __rv.__i;
}

__extension__ static __inline poly8x16x4_t __attribute__ ((__always_inline__))
vld4q_p8 (const poly8_t * __a)
{
  union { poly8x16x4_t __i; __builtin_neon_xi __o; } __rv;
  __rv.__o = __builtin_neon_vld4v16qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline poly16x8x4_t __attribute__ ((__always_inline__))
vld4q_p16 (const poly16_t * __a)
{
  union { poly16x8x4_t __i; __builtin_neon_xi __o; } __rv;
  __rv.__o = __builtin_neon_vld4v8hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline int8x8x4_t __attribute__ ((__always_inline__))
vld4_lane_s8 (const int8_t * __a, int8x8x4_t __b, const int __c)
{
  union { int8x8x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  union { int8x8x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_lanev8qi ((const __builtin_neon_qi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline int16x4x4_t __attribute__ ((__always_inline__))
vld4_lane_s16 (const int16_t * __a, int16x4x4_t __b, const int __c)
{
  union { int16x4x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  union { int16x4x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_lanev4hi ((const __builtin_neon_hi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline int32x2x4_t __attribute__ ((__always_inline__))
vld4_lane_s32 (const int32_t * __a, int32x2x4_t __b, const int __c)
{
  union { int32x2x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  union { int32x2x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_lanev2si ((const __builtin_neon_si *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline float32x2x4_t __attribute__ ((__always_inline__))
vld4_lane_f32 (const float32_t * __a, float32x2x4_t __b, const int __c)
{
  union { float32x2x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  union { float32x2x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_lanev2sf ((const __builtin_neon_sf *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline uint8x8x4_t __attribute__ ((__always_inline__))
vld4_lane_u8 (const uint8_t * __a, uint8x8x4_t __b, const int __c)
{
  union { uint8x8x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  union { uint8x8x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_lanev8qi ((const __builtin_neon_qi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline uint16x4x4_t __attribute__ ((__always_inline__))
vld4_lane_u16 (const uint16_t * __a, uint16x4x4_t __b, const int __c)
{
  union { uint16x4x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  union { uint16x4x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_lanev4hi ((const __builtin_neon_hi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline uint32x2x4_t __attribute__ ((__always_inline__))
vld4_lane_u32 (const uint32_t * __a, uint32x2x4_t __b, const int __c)
{
  union { uint32x2x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  union { uint32x2x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_lanev2si ((const __builtin_neon_si *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline poly8x8x4_t __attribute__ ((__always_inline__))
vld4_lane_p8 (const poly8_t * __a, poly8x8x4_t __b, const int __c)
{
  union { poly8x8x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  union { poly8x8x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_lanev8qi ((const __builtin_neon_qi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline poly16x4x4_t __attribute__ ((__always_inline__))
vld4_lane_p16 (const poly16_t * __a, poly16x4x4_t __b, const int __c)
{
  union { poly16x4x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  union { poly16x4x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_lanev4hi ((const __builtin_neon_hi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline int16x8x4_t __attribute__ ((__always_inline__))
vld4q_lane_s16 (const int16_t * __a, int16x8x4_t __b, const int __c)
{
  union { int16x8x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  union { int16x8x4_t __i; __builtin_neon_xi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_lanev8hi ((const __builtin_neon_hi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline int32x4x4_t __attribute__ ((__always_inline__))
vld4q_lane_s32 (const int32_t * __a, int32x4x4_t __b, const int __c)
{
  union { int32x4x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  union { int32x4x4_t __i; __builtin_neon_xi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_lanev4si ((const __builtin_neon_si *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline float32x4x4_t __attribute__ ((__always_inline__))
vld4q_lane_f32 (const float32_t * __a, float32x4x4_t __b, const int __c)
{
  union { float32x4x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  union { float32x4x4_t __i; __builtin_neon_xi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_lanev4sf ((const __builtin_neon_sf *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline uint16x8x4_t __attribute__ ((__always_inline__))
vld4q_lane_u16 (const uint16_t * __a, uint16x8x4_t __b, const int __c)
{
  union { uint16x8x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  union { uint16x8x4_t __i; __builtin_neon_xi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_lanev8hi ((const __builtin_neon_hi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline uint32x4x4_t __attribute__ ((__always_inline__))
vld4q_lane_u32 (const uint32_t * __a, uint32x4x4_t __b, const int __c)
{
  union { uint32x4x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  union { uint32x4x4_t __i; __builtin_neon_xi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_lanev4si ((const __builtin_neon_si *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline poly16x8x4_t __attribute__ ((__always_inline__))
vld4q_lane_p16 (const poly16_t * __a, poly16x8x4_t __b, const int __c)
{
  union { poly16x8x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  union { poly16x8x4_t __i; __builtin_neon_xi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_lanev8hi ((const __builtin_neon_hi *) __a, __bu.__o, __c);
  return __rv.__i;
}

__extension__ static __inline int8x8x4_t __attribute__ ((__always_inline__))
vld4_dup_s8 (const int8_t * __a)
{
  union { int8x8x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_dupv8qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline int16x4x4_t __attribute__ ((__always_inline__))
vld4_dup_s16 (const int16_t * __a)
{
  union { int16x4x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_dupv4hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline int32x2x4_t __attribute__ ((__always_inline__))
vld4_dup_s32 (const int32_t * __a)
{
  union { int32x2x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_dupv2si ((const __builtin_neon_si *) __a);
  return __rv.__i;
}

__extension__ static __inline float32x2x4_t __attribute__ ((__always_inline__))
vld4_dup_f32 (const float32_t * __a)
{
  union { float32x2x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_dupv2sf ((const __builtin_neon_sf *) __a);
  return __rv.__i;
}

__extension__ static __inline uint8x8x4_t __attribute__ ((__always_inline__))
vld4_dup_u8 (const uint8_t * __a)
{
  union { uint8x8x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_dupv8qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline uint16x4x4_t __attribute__ ((__always_inline__))
vld4_dup_u16 (const uint16_t * __a)
{
  union { uint16x4x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_dupv4hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline uint32x2x4_t __attribute__ ((__always_inline__))
vld4_dup_u32 (const uint32_t * __a)
{
  union { uint32x2x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_dupv2si ((const __builtin_neon_si *) __a);
  return __rv.__i;
}

__extension__ static __inline poly8x8x4_t __attribute__ ((__always_inline__))
vld4_dup_p8 (const poly8_t * __a)
{
  union { poly8x8x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_dupv8qi ((const __builtin_neon_qi *) __a);
  return __rv.__i;
}

__extension__ static __inline poly16x4x4_t __attribute__ ((__always_inline__))
vld4_dup_p16 (const poly16_t * __a)
{
  union { poly16x4x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_dupv4hi ((const __builtin_neon_hi *) __a);
  return __rv.__i;
}

__extension__ static __inline int64x1x4_t __attribute__ ((__always_inline__))
vld4_dup_s64 (const int64_t * __a)
{
  union { int64x1x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_dupdi ((const __builtin_neon_di *) __a);
  return __rv.__i;
}

__extension__ static __inline uint64x1x4_t __attribute__ ((__always_inline__))
vld4_dup_u64 (const uint64_t * __a)
{
  union { uint64x1x4_t __i; __builtin_neon_oi __o; } __rv;
  __rv.__o = __builtin_neon_vld4_dupdi ((const __builtin_neon_di *) __a);
  return __rv.__i;
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4_s8 (int8_t * __a, int8x8x4_t __b)
{
  union { int8x8x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst4v8qi ((__builtin_neon_qi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4_s16 (int16_t * __a, int16x4x4_t __b)
{
  union { int16x4x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst4v4hi ((__builtin_neon_hi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4_s32 (int32_t * __a, int32x2x4_t __b)
{
  union { int32x2x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst4v2si ((__builtin_neon_si *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4_f32 (float32_t * __a, float32x2x4_t __b)
{
  union { float32x2x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst4v2sf ((__builtin_neon_sf *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4_u8 (uint8_t * __a, uint8x8x4_t __b)
{
  union { uint8x8x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst4v8qi ((__builtin_neon_qi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4_u16 (uint16_t * __a, uint16x4x4_t __b)
{
  union { uint16x4x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst4v4hi ((__builtin_neon_hi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4_u32 (uint32_t * __a, uint32x2x4_t __b)
{
  union { uint32x2x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst4v2si ((__builtin_neon_si *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4_p8 (poly8_t * __a, poly8x8x4_t __b)
{
  union { poly8x8x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst4v8qi ((__builtin_neon_qi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4_p16 (poly16_t * __a, poly16x4x4_t __b)
{
  union { poly16x4x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst4v4hi ((__builtin_neon_hi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4_s64 (int64_t * __a, int64x1x4_t __b)
{
  union { int64x1x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst4di ((__builtin_neon_di *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4_u64 (uint64_t * __a, uint64x1x4_t __b)
{
  union { uint64x1x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst4di ((__builtin_neon_di *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4q_s8 (int8_t * __a, int8x16x4_t __b)
{
  union { int8x16x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  __builtin_neon_vst4v16qi ((__builtin_neon_qi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4q_s16 (int16_t * __a, int16x8x4_t __b)
{
  union { int16x8x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  __builtin_neon_vst4v8hi ((__builtin_neon_hi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4q_s32 (int32_t * __a, int32x4x4_t __b)
{
  union { int32x4x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  __builtin_neon_vst4v4si ((__builtin_neon_si *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4q_f32 (float32_t * __a, float32x4x4_t __b)
{
  union { float32x4x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  __builtin_neon_vst4v4sf ((__builtin_neon_sf *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4q_u8 (uint8_t * __a, uint8x16x4_t __b)
{
  union { uint8x16x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  __builtin_neon_vst4v16qi ((__builtin_neon_qi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4q_u16 (uint16_t * __a, uint16x8x4_t __b)
{
  union { uint16x8x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  __builtin_neon_vst4v8hi ((__builtin_neon_hi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4q_u32 (uint32_t * __a, uint32x4x4_t __b)
{
  union { uint32x4x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  __builtin_neon_vst4v4si ((__builtin_neon_si *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4q_p8 (poly8_t * __a, poly8x16x4_t __b)
{
  union { poly8x16x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  __builtin_neon_vst4v16qi ((__builtin_neon_qi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4q_p16 (poly16_t * __a, poly16x8x4_t __b)
{
  union { poly16x8x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  __builtin_neon_vst4v8hi ((__builtin_neon_hi *) __a, __bu.__o);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4_lane_s8 (int8_t * __a, int8x8x4_t __b, const int __c)
{
  union { int8x8x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst4_lanev8qi ((__builtin_neon_qi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4_lane_s16 (int16_t * __a, int16x4x4_t __b, const int __c)
{
  union { int16x4x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst4_lanev4hi ((__builtin_neon_hi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4_lane_s32 (int32_t * __a, int32x2x4_t __b, const int __c)
{
  union { int32x2x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst4_lanev2si ((__builtin_neon_si *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4_lane_f32 (float32_t * __a, float32x2x4_t __b, const int __c)
{
  union { float32x2x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst4_lanev2sf ((__builtin_neon_sf *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4_lane_u8 (uint8_t * __a, uint8x8x4_t __b, const int __c)
{
  union { uint8x8x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst4_lanev8qi ((__builtin_neon_qi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4_lane_u16 (uint16_t * __a, uint16x4x4_t __b, const int __c)
{
  union { uint16x4x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst4_lanev4hi ((__builtin_neon_hi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4_lane_u32 (uint32_t * __a, uint32x2x4_t __b, const int __c)
{
  union { uint32x2x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst4_lanev2si ((__builtin_neon_si *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4_lane_p8 (poly8_t * __a, poly8x8x4_t __b, const int __c)
{
  union { poly8x8x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst4_lanev8qi ((__builtin_neon_qi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4_lane_p16 (poly16_t * __a, poly16x4x4_t __b, const int __c)
{
  union { poly16x4x4_t __i; __builtin_neon_oi __o; } __bu = { __b };
  __builtin_neon_vst4_lanev4hi ((__builtin_neon_hi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4q_lane_s16 (int16_t * __a, int16x8x4_t __b, const int __c)
{
  union { int16x8x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  __builtin_neon_vst4_lanev8hi ((__builtin_neon_hi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4q_lane_s32 (int32_t * __a, int32x4x4_t __b, const int __c)
{
  union { int32x4x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  __builtin_neon_vst4_lanev4si ((__builtin_neon_si *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4q_lane_f32 (float32_t * __a, float32x4x4_t __b, const int __c)
{
  union { float32x4x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  __builtin_neon_vst4_lanev4sf ((__builtin_neon_sf *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4q_lane_u16 (uint16_t * __a, uint16x8x4_t __b, const int __c)
{
  union { uint16x8x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  __builtin_neon_vst4_lanev8hi ((__builtin_neon_hi *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4q_lane_u32 (uint32_t * __a, uint32x4x4_t __b, const int __c)
{
  union { uint32x4x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  __builtin_neon_vst4_lanev4si ((__builtin_neon_si *) __a, __bu.__o, __c);
}

__extension__ static __inline void __attribute__ ((__always_inline__))
vst4q_lane_p16 (poly16_t * __a, poly16x8x4_t __b, const int __c)
{
  union { poly16x8x4_t __i; __builtin_neon_xi __o; } __bu = { __b };
  __builtin_neon_vst4_lanev8hi ((__builtin_neon_hi *) __a, __bu.__o, __c);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vand_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vandv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vand_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vandv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vand_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vandv2si (__a, __b, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vand_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vandv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vand_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vandv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vand_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vandv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vand_s64 (int64x1_t __a, int64x1_t __b)
{
  return (int64x1_t)__builtin_neon_vanddi (__a, __b, 1);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vand_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return (uint64x1_t)__builtin_neon_vanddi ((int64x1_t) __a, (int64x1_t) __b, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vandq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t)__builtin_neon_vandv16qi (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vandq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vandv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vandq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vandv4si (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vandq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (int64x2_t)__builtin_neon_vandv2di (__a, __b, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vandq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vandv16qi ((int8x16_t) __a, (int8x16_t) __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vandq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vandv8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vandq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vandv4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vandq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return (uint64x2_t)__builtin_neon_vandv2di ((int64x2_t) __a, (int64x2_t) __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vorr_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vorrv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vorr_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vorrv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vorr_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vorrv2si (__a, __b, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vorr_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vorrv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vorr_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vorrv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vorr_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vorrv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vorr_s64 (int64x1_t __a, int64x1_t __b)
{
  return (int64x1_t)__builtin_neon_vorrdi (__a, __b, 1);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vorr_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return (uint64x1_t)__builtin_neon_vorrdi ((int64x1_t) __a, (int64x1_t) __b, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vorrq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t)__builtin_neon_vorrv16qi (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vorrq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vorrv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vorrq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vorrv4si (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vorrq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (int64x2_t)__builtin_neon_vorrv2di (__a, __b, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vorrq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vorrv16qi ((int8x16_t) __a, (int8x16_t) __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vorrq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vorrv8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vorrq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vorrv4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vorrq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return (uint64x2_t)__builtin_neon_vorrv2di ((int64x2_t) __a, (int64x2_t) __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
veor_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_veorv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
veor_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_veorv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
veor_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_veorv2si (__a, __b, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
veor_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_veorv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
veor_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_veorv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
veor_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_veorv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
veor_s64 (int64x1_t __a, int64x1_t __b)
{
  return (int64x1_t)__builtin_neon_veordi (__a, __b, 1);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
veor_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return (uint64x1_t)__builtin_neon_veordi ((int64x1_t) __a, (int64x1_t) __b, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
veorq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t)__builtin_neon_veorv16qi (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
veorq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_veorv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
veorq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_veorv4si (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
veorq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (int64x2_t)__builtin_neon_veorv2di (__a, __b, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
veorq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_veorv16qi ((int8x16_t) __a, (int8x16_t) __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
veorq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_veorv8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
veorq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_veorv4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
veorq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return (uint64x2_t)__builtin_neon_veorv2di ((int64x2_t) __a, (int64x2_t) __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vbic_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vbicv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vbic_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vbicv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vbic_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vbicv2si (__a, __b, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vbic_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vbicv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vbic_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vbicv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vbic_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vbicv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vbic_s64 (int64x1_t __a, int64x1_t __b)
{
  return (int64x1_t)__builtin_neon_vbicdi (__a, __b, 1);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vbic_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return (uint64x1_t)__builtin_neon_vbicdi ((int64x1_t) __a, (int64x1_t) __b, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vbicq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t)__builtin_neon_vbicv16qi (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vbicq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vbicv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vbicq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vbicv4si (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vbicq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (int64x2_t)__builtin_neon_vbicv2di (__a, __b, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vbicq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vbicv16qi ((int8x16_t) __a, (int8x16_t) __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vbicq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vbicv8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vbicq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vbicv4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vbicq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return (uint64x2_t)__builtin_neon_vbicv2di ((int64x2_t) __a, (int64x2_t) __b, 0);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vorn_s8 (int8x8_t __a, int8x8_t __b)
{
  return (int8x8_t)__builtin_neon_vornv8qi (__a, __b, 1);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vorn_s16 (int16x4_t __a, int16x4_t __b)
{
  return (int16x4_t)__builtin_neon_vornv4hi (__a, __b, 1);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vorn_s32 (int32x2_t __a, int32x2_t __b)
{
  return (int32x2_t)__builtin_neon_vornv2si (__a, __b, 1);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vorn_u8 (uint8x8_t __a, uint8x8_t __b)
{
  return (uint8x8_t)__builtin_neon_vornv8qi ((int8x8_t) __a, (int8x8_t) __b, 0);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vorn_u16 (uint16x4_t __a, uint16x4_t __b)
{
  return (uint16x4_t)__builtin_neon_vornv4hi ((int16x4_t) __a, (int16x4_t) __b, 0);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vorn_u32 (uint32x2_t __a, uint32x2_t __b)
{
  return (uint32x2_t)__builtin_neon_vornv2si ((int32x2_t) __a, (int32x2_t) __b, 0);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vorn_s64 (int64x1_t __a, int64x1_t __b)
{
  return (int64x1_t)__builtin_neon_vorndi (__a, __b, 1);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vorn_u64 (uint64x1_t __a, uint64x1_t __b)
{
  return (uint64x1_t)__builtin_neon_vorndi ((int64x1_t) __a, (int64x1_t) __b, 0);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vornq_s8 (int8x16_t __a, int8x16_t __b)
{
  return (int8x16_t)__builtin_neon_vornv16qi (__a, __b, 1);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vornq_s16 (int16x8_t __a, int16x8_t __b)
{
  return (int16x8_t)__builtin_neon_vornv8hi (__a, __b, 1);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vornq_s32 (int32x4_t __a, int32x4_t __b)
{
  return (int32x4_t)__builtin_neon_vornv4si (__a, __b, 1);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vornq_s64 (int64x2_t __a, int64x2_t __b)
{
  return (int64x2_t)__builtin_neon_vornv2di (__a, __b, 1);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vornq_u8 (uint8x16_t __a, uint8x16_t __b)
{
  return (uint8x16_t)__builtin_neon_vornv16qi ((int8x16_t) __a, (int8x16_t) __b, 0);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vornq_u16 (uint16x8_t __a, uint16x8_t __b)
{
  return (uint16x8_t)__builtin_neon_vornv8hi ((int16x8_t) __a, (int16x8_t) __b, 0);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vornq_u32 (uint32x4_t __a, uint32x4_t __b)
{
  return (uint32x4_t)__builtin_neon_vornv4si ((int32x4_t) __a, (int32x4_t) __b, 0);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vornq_u64 (uint64x2_t __a, uint64x2_t __b)
{
  return (uint64x2_t)__builtin_neon_vornv2di ((int64x2_t) __a, (int64x2_t) __b, 0);
}


__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vreinterpret_p8_s8 (int8x8_t __a)
{
  return (poly8x8_t)__builtin_neon_vreinterpretv8qiv8qi (__a);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vreinterpret_p8_s16 (int16x4_t __a)
{
  return (poly8x8_t)__builtin_neon_vreinterpretv8qiv4hi (__a);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vreinterpret_p8_s32 (int32x2_t __a)
{
  return (poly8x8_t)__builtin_neon_vreinterpretv8qiv2si (__a);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vreinterpret_p8_s64 (int64x1_t __a)
{
  return (poly8x8_t)__builtin_neon_vreinterpretv8qidi (__a);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vreinterpret_p8_f32 (float32x2_t __a)
{
  return (poly8x8_t)__builtin_neon_vreinterpretv8qiv2sf (__a);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vreinterpret_p8_u8 (uint8x8_t __a)
{
  return (poly8x8_t)__builtin_neon_vreinterpretv8qiv8qi ((int8x8_t) __a);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vreinterpret_p8_u16 (uint16x4_t __a)
{
  return (poly8x8_t)__builtin_neon_vreinterpretv8qiv4hi ((int16x4_t) __a);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vreinterpret_p8_u32 (uint32x2_t __a)
{
  return (poly8x8_t)__builtin_neon_vreinterpretv8qiv2si ((int32x2_t) __a);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vreinterpret_p8_u64 (uint64x1_t __a)
{
  return (poly8x8_t)__builtin_neon_vreinterpretv8qidi ((int64x1_t) __a);
}

__extension__ static __inline poly8x8_t __attribute__ ((__always_inline__))
vreinterpret_p8_p16 (poly16x4_t __a)
{
  return (poly8x8_t)__builtin_neon_vreinterpretv8qiv4hi ((int16x4_t) __a);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vreinterpretq_p8_s8 (int8x16_t __a)
{
  return (poly8x16_t)__builtin_neon_vreinterpretv16qiv16qi (__a);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vreinterpretq_p8_s16 (int16x8_t __a)
{
  return (poly8x16_t)__builtin_neon_vreinterpretv16qiv8hi (__a);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vreinterpretq_p8_s32 (int32x4_t __a)
{
  return (poly8x16_t)__builtin_neon_vreinterpretv16qiv4si (__a);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vreinterpretq_p8_s64 (int64x2_t __a)
{
  return (poly8x16_t)__builtin_neon_vreinterpretv16qiv2di (__a);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vreinterpretq_p8_f32 (float32x4_t __a)
{
  return (poly8x16_t)__builtin_neon_vreinterpretv16qiv4sf (__a);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vreinterpretq_p8_u8 (uint8x16_t __a)
{
  return (poly8x16_t)__builtin_neon_vreinterpretv16qiv16qi ((int8x16_t) __a);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vreinterpretq_p8_u16 (uint16x8_t __a)
{
  return (poly8x16_t)__builtin_neon_vreinterpretv16qiv8hi ((int16x8_t) __a);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vreinterpretq_p8_u32 (uint32x4_t __a)
{
  return (poly8x16_t)__builtin_neon_vreinterpretv16qiv4si ((int32x4_t) __a);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vreinterpretq_p8_u64 (uint64x2_t __a)
{
  return (poly8x16_t)__builtin_neon_vreinterpretv16qiv2di ((int64x2_t) __a);
}

__extension__ static __inline poly8x16_t __attribute__ ((__always_inline__))
vreinterpretq_p8_p16 (poly16x8_t __a)
{
  return (poly8x16_t)__builtin_neon_vreinterpretv16qiv8hi ((int16x8_t) __a);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vreinterpret_p16_s8 (int8x8_t __a)
{
  return (poly16x4_t)__builtin_neon_vreinterpretv4hiv8qi (__a);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vreinterpret_p16_s16 (int16x4_t __a)
{
  return (poly16x4_t)__builtin_neon_vreinterpretv4hiv4hi (__a);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vreinterpret_p16_s32 (int32x2_t __a)
{
  return (poly16x4_t)__builtin_neon_vreinterpretv4hiv2si (__a);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vreinterpret_p16_s64 (int64x1_t __a)
{
  return (poly16x4_t)__builtin_neon_vreinterpretv4hidi (__a);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vreinterpret_p16_f32 (float32x2_t __a)
{
  return (poly16x4_t)__builtin_neon_vreinterpretv4hiv2sf (__a);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vreinterpret_p16_u8 (uint8x8_t __a)
{
  return (poly16x4_t)__builtin_neon_vreinterpretv4hiv8qi ((int8x8_t) __a);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vreinterpret_p16_u16 (uint16x4_t __a)
{
  return (poly16x4_t)__builtin_neon_vreinterpretv4hiv4hi ((int16x4_t) __a);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vreinterpret_p16_u32 (uint32x2_t __a)
{
  return (poly16x4_t)__builtin_neon_vreinterpretv4hiv2si ((int32x2_t) __a);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vreinterpret_p16_u64 (uint64x1_t __a)
{
  return (poly16x4_t)__builtin_neon_vreinterpretv4hidi ((int64x1_t) __a);
}

__extension__ static __inline poly16x4_t __attribute__ ((__always_inline__))
vreinterpret_p16_p8 (poly8x8_t __a)
{
  return (poly16x4_t)__builtin_neon_vreinterpretv4hiv8qi ((int8x8_t) __a);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vreinterpretq_p16_s8 (int8x16_t __a)
{
  return (poly16x8_t)__builtin_neon_vreinterpretv8hiv16qi (__a);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vreinterpretq_p16_s16 (int16x8_t __a)
{
  return (poly16x8_t)__builtin_neon_vreinterpretv8hiv8hi (__a);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vreinterpretq_p16_s32 (int32x4_t __a)
{
  return (poly16x8_t)__builtin_neon_vreinterpretv8hiv4si (__a);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vreinterpretq_p16_s64 (int64x2_t __a)
{
  return (poly16x8_t)__builtin_neon_vreinterpretv8hiv2di (__a);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vreinterpretq_p16_f32 (float32x4_t __a)
{
  return (poly16x8_t)__builtin_neon_vreinterpretv8hiv4sf (__a);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vreinterpretq_p16_u8 (uint8x16_t __a)
{
  return (poly16x8_t)__builtin_neon_vreinterpretv8hiv16qi ((int8x16_t) __a);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vreinterpretq_p16_u16 (uint16x8_t __a)
{
  return (poly16x8_t)__builtin_neon_vreinterpretv8hiv8hi ((int16x8_t) __a);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vreinterpretq_p16_u32 (uint32x4_t __a)
{
  return (poly16x8_t)__builtin_neon_vreinterpretv8hiv4si ((int32x4_t) __a);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vreinterpretq_p16_u64 (uint64x2_t __a)
{
  return (poly16x8_t)__builtin_neon_vreinterpretv8hiv2di ((int64x2_t) __a);
}

__extension__ static __inline poly16x8_t __attribute__ ((__always_inline__))
vreinterpretq_p16_p8 (poly8x16_t __a)
{
  return (poly16x8_t)__builtin_neon_vreinterpretv8hiv16qi ((int8x16_t) __a);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vreinterpret_f32_s8 (int8x8_t __a)
{
  return (float32x2_t)__builtin_neon_vreinterpretv2sfv8qi (__a);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vreinterpret_f32_s16 (int16x4_t __a)
{
  return (float32x2_t)__builtin_neon_vreinterpretv2sfv4hi (__a);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vreinterpret_f32_s32 (int32x2_t __a)
{
  return (float32x2_t)__builtin_neon_vreinterpretv2sfv2si (__a);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vreinterpret_f32_s64 (int64x1_t __a)
{
  return (float32x2_t)__builtin_neon_vreinterpretv2sfdi (__a);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vreinterpret_f32_u8 (uint8x8_t __a)
{
  return (float32x2_t)__builtin_neon_vreinterpretv2sfv8qi ((int8x8_t) __a);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vreinterpret_f32_u16 (uint16x4_t __a)
{
  return (float32x2_t)__builtin_neon_vreinterpretv2sfv4hi ((int16x4_t) __a);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vreinterpret_f32_u32 (uint32x2_t __a)
{
  return (float32x2_t)__builtin_neon_vreinterpretv2sfv2si ((int32x2_t) __a);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vreinterpret_f32_u64 (uint64x1_t __a)
{
  return (float32x2_t)__builtin_neon_vreinterpretv2sfdi ((int64x1_t) __a);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vreinterpret_f32_p8 (poly8x8_t __a)
{
  return (float32x2_t)__builtin_neon_vreinterpretv2sfv8qi ((int8x8_t) __a);
}

__extension__ static __inline float32x2_t __attribute__ ((__always_inline__))
vreinterpret_f32_p16 (poly16x4_t __a)
{
  return (float32x2_t)__builtin_neon_vreinterpretv2sfv4hi ((int16x4_t) __a);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vreinterpretq_f32_s8 (int8x16_t __a)
{
  return (float32x4_t)__builtin_neon_vreinterpretv4sfv16qi (__a);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vreinterpretq_f32_s16 (int16x8_t __a)
{
  return (float32x4_t)__builtin_neon_vreinterpretv4sfv8hi (__a);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vreinterpretq_f32_s32 (int32x4_t __a)
{
  return (float32x4_t)__builtin_neon_vreinterpretv4sfv4si (__a);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vreinterpretq_f32_s64 (int64x2_t __a)
{
  return (float32x4_t)__builtin_neon_vreinterpretv4sfv2di (__a);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vreinterpretq_f32_u8 (uint8x16_t __a)
{
  return (float32x4_t)__builtin_neon_vreinterpretv4sfv16qi ((int8x16_t) __a);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vreinterpretq_f32_u16 (uint16x8_t __a)
{
  return (float32x4_t)__builtin_neon_vreinterpretv4sfv8hi ((int16x8_t) __a);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vreinterpretq_f32_u32 (uint32x4_t __a)
{
  return (float32x4_t)__builtin_neon_vreinterpretv4sfv4si ((int32x4_t) __a);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vreinterpretq_f32_u64 (uint64x2_t __a)
{
  return (float32x4_t)__builtin_neon_vreinterpretv4sfv2di ((int64x2_t) __a);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vreinterpretq_f32_p8 (poly8x16_t __a)
{
  return (float32x4_t)__builtin_neon_vreinterpretv4sfv16qi ((int8x16_t) __a);
}

__extension__ static __inline float32x4_t __attribute__ ((__always_inline__))
vreinterpretq_f32_p16 (poly16x8_t __a)
{
  return (float32x4_t)__builtin_neon_vreinterpretv4sfv8hi ((int16x8_t) __a);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vreinterpret_s64_s8 (int8x8_t __a)
{
  return (int64x1_t)__builtin_neon_vreinterpretdiv8qi (__a);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vreinterpret_s64_s16 (int16x4_t __a)
{
  return (int64x1_t)__builtin_neon_vreinterpretdiv4hi (__a);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vreinterpret_s64_s32 (int32x2_t __a)
{
  return (int64x1_t)__builtin_neon_vreinterpretdiv2si (__a);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vreinterpret_s64_f32 (float32x2_t __a)
{
  return (int64x1_t)__builtin_neon_vreinterpretdiv2sf (__a);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vreinterpret_s64_u8 (uint8x8_t __a)
{
  return (int64x1_t)__builtin_neon_vreinterpretdiv8qi ((int8x8_t) __a);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vreinterpret_s64_u16 (uint16x4_t __a)
{
  return (int64x1_t)__builtin_neon_vreinterpretdiv4hi ((int16x4_t) __a);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vreinterpret_s64_u32 (uint32x2_t __a)
{
  return (int64x1_t)__builtin_neon_vreinterpretdiv2si ((int32x2_t) __a);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vreinterpret_s64_u64 (uint64x1_t __a)
{
  return (int64x1_t)__builtin_neon_vreinterpretdidi ((int64x1_t) __a);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vreinterpret_s64_p8 (poly8x8_t __a)
{
  return (int64x1_t)__builtin_neon_vreinterpretdiv8qi ((int8x8_t) __a);
}

__extension__ static __inline int64x1_t __attribute__ ((__always_inline__))
vreinterpret_s64_p16 (poly16x4_t __a)
{
  return (int64x1_t)__builtin_neon_vreinterpretdiv4hi ((int16x4_t) __a);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vreinterpretq_s64_s8 (int8x16_t __a)
{
  return (int64x2_t)__builtin_neon_vreinterpretv2div16qi (__a);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vreinterpretq_s64_s16 (int16x8_t __a)
{
  return (int64x2_t)__builtin_neon_vreinterpretv2div8hi (__a);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vreinterpretq_s64_s32 (int32x4_t __a)
{
  return (int64x2_t)__builtin_neon_vreinterpretv2div4si (__a);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vreinterpretq_s64_f32 (float32x4_t __a)
{
  return (int64x2_t)__builtin_neon_vreinterpretv2div4sf (__a);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vreinterpretq_s64_u8 (uint8x16_t __a)
{
  return (int64x2_t)__builtin_neon_vreinterpretv2div16qi ((int8x16_t) __a);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vreinterpretq_s64_u16 (uint16x8_t __a)
{
  return (int64x2_t)__builtin_neon_vreinterpretv2div8hi ((int16x8_t) __a);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vreinterpretq_s64_u32 (uint32x4_t __a)
{
  return (int64x2_t)__builtin_neon_vreinterpretv2div4si ((int32x4_t) __a);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vreinterpretq_s64_u64 (uint64x2_t __a)
{
  return (int64x2_t)__builtin_neon_vreinterpretv2div2di ((int64x2_t) __a);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vreinterpretq_s64_p8 (poly8x16_t __a)
{
  return (int64x2_t)__builtin_neon_vreinterpretv2div16qi ((int8x16_t) __a);
}

__extension__ static __inline int64x2_t __attribute__ ((__always_inline__))
vreinterpretq_s64_p16 (poly16x8_t __a)
{
  return (int64x2_t)__builtin_neon_vreinterpretv2div8hi ((int16x8_t) __a);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vreinterpret_u64_s8 (int8x8_t __a)
{
  return (uint64x1_t)__builtin_neon_vreinterpretdiv8qi (__a);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vreinterpret_u64_s16 (int16x4_t __a)
{
  return (uint64x1_t)__builtin_neon_vreinterpretdiv4hi (__a);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vreinterpret_u64_s32 (int32x2_t __a)
{
  return (uint64x1_t)__builtin_neon_vreinterpretdiv2si (__a);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vreinterpret_u64_s64 (int64x1_t __a)
{
  return (uint64x1_t)__builtin_neon_vreinterpretdidi (__a);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vreinterpret_u64_f32 (float32x2_t __a)
{
  return (uint64x1_t)__builtin_neon_vreinterpretdiv2sf (__a);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vreinterpret_u64_u8 (uint8x8_t __a)
{
  return (uint64x1_t)__builtin_neon_vreinterpretdiv8qi ((int8x8_t) __a);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vreinterpret_u64_u16 (uint16x4_t __a)
{
  return (uint64x1_t)__builtin_neon_vreinterpretdiv4hi ((int16x4_t) __a);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vreinterpret_u64_u32 (uint32x2_t __a)
{
  return (uint64x1_t)__builtin_neon_vreinterpretdiv2si ((int32x2_t) __a);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vreinterpret_u64_p8 (poly8x8_t __a)
{
  return (uint64x1_t)__builtin_neon_vreinterpretdiv8qi ((int8x8_t) __a);
}

__extension__ static __inline uint64x1_t __attribute__ ((__always_inline__))
vreinterpret_u64_p16 (poly16x4_t __a)
{
  return (uint64x1_t)__builtin_neon_vreinterpretdiv4hi ((int16x4_t) __a);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vreinterpretq_u64_s8 (int8x16_t __a)
{
  return (uint64x2_t)__builtin_neon_vreinterpretv2div16qi (__a);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vreinterpretq_u64_s16 (int16x8_t __a)
{
  return (uint64x2_t)__builtin_neon_vreinterpretv2div8hi (__a);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vreinterpretq_u64_s32 (int32x4_t __a)
{
  return (uint64x2_t)__builtin_neon_vreinterpretv2div4si (__a);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vreinterpretq_u64_s64 (int64x2_t __a)
{
  return (uint64x2_t)__builtin_neon_vreinterpretv2div2di (__a);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vreinterpretq_u64_f32 (float32x4_t __a)
{
  return (uint64x2_t)__builtin_neon_vreinterpretv2div4sf (__a);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vreinterpretq_u64_u8 (uint8x16_t __a)
{
  return (uint64x2_t)__builtin_neon_vreinterpretv2div16qi ((int8x16_t) __a);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vreinterpretq_u64_u16 (uint16x8_t __a)
{
  return (uint64x2_t)__builtin_neon_vreinterpretv2div8hi ((int16x8_t) __a);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vreinterpretq_u64_u32 (uint32x4_t __a)
{
  return (uint64x2_t)__builtin_neon_vreinterpretv2div4si ((int32x4_t) __a);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vreinterpretq_u64_p8 (poly8x16_t __a)
{
  return (uint64x2_t)__builtin_neon_vreinterpretv2div16qi ((int8x16_t) __a);
}

__extension__ static __inline uint64x2_t __attribute__ ((__always_inline__))
vreinterpretq_u64_p16 (poly16x8_t __a)
{
  return (uint64x2_t)__builtin_neon_vreinterpretv2div8hi ((int16x8_t) __a);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vreinterpret_s8_s16 (int16x4_t __a)
{
  return (int8x8_t)__builtin_neon_vreinterpretv8qiv4hi (__a);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vreinterpret_s8_s32 (int32x2_t __a)
{
  return (int8x8_t)__builtin_neon_vreinterpretv8qiv2si (__a);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vreinterpret_s8_s64 (int64x1_t __a)
{
  return (int8x8_t)__builtin_neon_vreinterpretv8qidi (__a);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vreinterpret_s8_f32 (float32x2_t __a)
{
  return (int8x8_t)__builtin_neon_vreinterpretv8qiv2sf (__a);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vreinterpret_s8_u8 (uint8x8_t __a)
{
  return (int8x8_t)__builtin_neon_vreinterpretv8qiv8qi ((int8x8_t) __a);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vreinterpret_s8_u16 (uint16x4_t __a)
{
  return (int8x8_t)__builtin_neon_vreinterpretv8qiv4hi ((int16x4_t) __a);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vreinterpret_s8_u32 (uint32x2_t __a)
{
  return (int8x8_t)__builtin_neon_vreinterpretv8qiv2si ((int32x2_t) __a);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vreinterpret_s8_u64 (uint64x1_t __a)
{
  return (int8x8_t)__builtin_neon_vreinterpretv8qidi ((int64x1_t) __a);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vreinterpret_s8_p8 (poly8x8_t __a)
{
  return (int8x8_t)__builtin_neon_vreinterpretv8qiv8qi ((int8x8_t) __a);
}

__extension__ static __inline int8x8_t __attribute__ ((__always_inline__))
vreinterpret_s8_p16 (poly16x4_t __a)
{
  return (int8x8_t)__builtin_neon_vreinterpretv8qiv4hi ((int16x4_t) __a);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vreinterpretq_s8_s16 (int16x8_t __a)
{
  return (int8x16_t)__builtin_neon_vreinterpretv16qiv8hi (__a);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vreinterpretq_s8_s32 (int32x4_t __a)
{
  return (int8x16_t)__builtin_neon_vreinterpretv16qiv4si (__a);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vreinterpretq_s8_s64 (int64x2_t __a)
{
  return (int8x16_t)__builtin_neon_vreinterpretv16qiv2di (__a);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vreinterpretq_s8_f32 (float32x4_t __a)
{
  return (int8x16_t)__builtin_neon_vreinterpretv16qiv4sf (__a);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vreinterpretq_s8_u8 (uint8x16_t __a)
{
  return (int8x16_t)__builtin_neon_vreinterpretv16qiv16qi ((int8x16_t) __a);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vreinterpretq_s8_u16 (uint16x8_t __a)
{
  return (int8x16_t)__builtin_neon_vreinterpretv16qiv8hi ((int16x8_t) __a);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vreinterpretq_s8_u32 (uint32x4_t __a)
{
  return (int8x16_t)__builtin_neon_vreinterpretv16qiv4si ((int32x4_t) __a);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vreinterpretq_s8_u64 (uint64x2_t __a)
{
  return (int8x16_t)__builtin_neon_vreinterpretv16qiv2di ((int64x2_t) __a);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vreinterpretq_s8_p8 (poly8x16_t __a)
{
  return (int8x16_t)__builtin_neon_vreinterpretv16qiv16qi ((int8x16_t) __a);
}

__extension__ static __inline int8x16_t __attribute__ ((__always_inline__))
vreinterpretq_s8_p16 (poly16x8_t __a)
{
  return (int8x16_t)__builtin_neon_vreinterpretv16qiv8hi ((int16x8_t) __a);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vreinterpret_s16_s8 (int8x8_t __a)
{
  return (int16x4_t)__builtin_neon_vreinterpretv4hiv8qi (__a);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vreinterpret_s16_s32 (int32x2_t __a)
{
  return (int16x4_t)__builtin_neon_vreinterpretv4hiv2si (__a);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vreinterpret_s16_s64 (int64x1_t __a)
{
  return (int16x4_t)__builtin_neon_vreinterpretv4hidi (__a);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vreinterpret_s16_f32 (float32x2_t __a)
{
  return (int16x4_t)__builtin_neon_vreinterpretv4hiv2sf (__a);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vreinterpret_s16_u8 (uint8x8_t __a)
{
  return (int16x4_t)__builtin_neon_vreinterpretv4hiv8qi ((int8x8_t) __a);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vreinterpret_s16_u16 (uint16x4_t __a)
{
  return (int16x4_t)__builtin_neon_vreinterpretv4hiv4hi ((int16x4_t) __a);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vreinterpret_s16_u32 (uint32x2_t __a)
{
  return (int16x4_t)__builtin_neon_vreinterpretv4hiv2si ((int32x2_t) __a);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vreinterpret_s16_u64 (uint64x1_t __a)
{
  return (int16x4_t)__builtin_neon_vreinterpretv4hidi ((int64x1_t) __a);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vreinterpret_s16_p8 (poly8x8_t __a)
{
  return (int16x4_t)__builtin_neon_vreinterpretv4hiv8qi ((int8x8_t) __a);
}

__extension__ static __inline int16x4_t __attribute__ ((__always_inline__))
vreinterpret_s16_p16 (poly16x4_t __a)
{
  return (int16x4_t)__builtin_neon_vreinterpretv4hiv4hi ((int16x4_t) __a);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vreinterpretq_s16_s8 (int8x16_t __a)
{
  return (int16x8_t)__builtin_neon_vreinterpretv8hiv16qi (__a);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vreinterpretq_s16_s32 (int32x4_t __a)
{
  return (int16x8_t)__builtin_neon_vreinterpretv8hiv4si (__a);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vreinterpretq_s16_s64 (int64x2_t __a)
{
  return (int16x8_t)__builtin_neon_vreinterpretv8hiv2di (__a);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vreinterpretq_s16_f32 (float32x4_t __a)
{
  return (int16x8_t)__builtin_neon_vreinterpretv8hiv4sf (__a);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vreinterpretq_s16_u8 (uint8x16_t __a)
{
  return (int16x8_t)__builtin_neon_vreinterpretv8hiv16qi ((int8x16_t) __a);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vreinterpretq_s16_u16 (uint16x8_t __a)
{
  return (int16x8_t)__builtin_neon_vreinterpretv8hiv8hi ((int16x8_t) __a);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vreinterpretq_s16_u32 (uint32x4_t __a)
{
  return (int16x8_t)__builtin_neon_vreinterpretv8hiv4si ((int32x4_t) __a);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vreinterpretq_s16_u64 (uint64x2_t __a)
{
  return (int16x8_t)__builtin_neon_vreinterpretv8hiv2di ((int64x2_t) __a);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vreinterpretq_s16_p8 (poly8x16_t __a)
{
  return (int16x8_t)__builtin_neon_vreinterpretv8hiv16qi ((int8x16_t) __a);
}

__extension__ static __inline int16x8_t __attribute__ ((__always_inline__))
vreinterpretq_s16_p16 (poly16x8_t __a)
{
  return (int16x8_t)__builtin_neon_vreinterpretv8hiv8hi ((int16x8_t) __a);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vreinterpret_s32_s8 (int8x8_t __a)
{
  return (int32x2_t)__builtin_neon_vreinterpretv2siv8qi (__a);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vreinterpret_s32_s16 (int16x4_t __a)
{
  return (int32x2_t)__builtin_neon_vreinterpretv2siv4hi (__a);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vreinterpret_s32_s64 (int64x1_t __a)
{
  return (int32x2_t)__builtin_neon_vreinterpretv2sidi (__a);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vreinterpret_s32_f32 (float32x2_t __a)
{
  return (int32x2_t)__builtin_neon_vreinterpretv2siv2sf (__a);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vreinterpret_s32_u8 (uint8x8_t __a)
{
  return (int32x2_t)__builtin_neon_vreinterpretv2siv8qi ((int8x8_t) __a);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vreinterpret_s32_u16 (uint16x4_t __a)
{
  return (int32x2_t)__builtin_neon_vreinterpretv2siv4hi ((int16x4_t) __a);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vreinterpret_s32_u32 (uint32x2_t __a)
{
  return (int32x2_t)__builtin_neon_vreinterpretv2siv2si ((int32x2_t) __a);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vreinterpret_s32_u64 (uint64x1_t __a)
{
  return (int32x2_t)__builtin_neon_vreinterpretv2sidi ((int64x1_t) __a);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vreinterpret_s32_p8 (poly8x8_t __a)
{
  return (int32x2_t)__builtin_neon_vreinterpretv2siv8qi ((int8x8_t) __a);
}

__extension__ static __inline int32x2_t __attribute__ ((__always_inline__))
vreinterpret_s32_p16 (poly16x4_t __a)
{
  return (int32x2_t)__builtin_neon_vreinterpretv2siv4hi ((int16x4_t) __a);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vreinterpretq_s32_s8 (int8x16_t __a)
{
  return (int32x4_t)__builtin_neon_vreinterpretv4siv16qi (__a);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vreinterpretq_s32_s16 (int16x8_t __a)
{
  return (int32x4_t)__builtin_neon_vreinterpretv4siv8hi (__a);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vreinterpretq_s32_s64 (int64x2_t __a)
{
  return (int32x4_t)__builtin_neon_vreinterpretv4siv2di (__a);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vreinterpretq_s32_f32 (float32x4_t __a)
{
  return (int32x4_t)__builtin_neon_vreinterpretv4siv4sf (__a);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vreinterpretq_s32_u8 (uint8x16_t __a)
{
  return (int32x4_t)__builtin_neon_vreinterpretv4siv16qi ((int8x16_t) __a);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vreinterpretq_s32_u16 (uint16x8_t __a)
{
  return (int32x4_t)__builtin_neon_vreinterpretv4siv8hi ((int16x8_t) __a);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vreinterpretq_s32_u32 (uint32x4_t __a)
{
  return (int32x4_t)__builtin_neon_vreinterpretv4siv4si ((int32x4_t) __a);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vreinterpretq_s32_u64 (uint64x2_t __a)
{
  return (int32x4_t)__builtin_neon_vreinterpretv4siv2di ((int64x2_t) __a);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vreinterpretq_s32_p8 (poly8x16_t __a)
{
  return (int32x4_t)__builtin_neon_vreinterpretv4siv16qi ((int8x16_t) __a);
}

__extension__ static __inline int32x4_t __attribute__ ((__always_inline__))
vreinterpretq_s32_p16 (poly16x8_t __a)
{
  return (int32x4_t)__builtin_neon_vreinterpretv4siv8hi ((int16x8_t) __a);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vreinterpret_u8_s8 (int8x8_t __a)
{
  return (uint8x8_t)__builtin_neon_vreinterpretv8qiv8qi (__a);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vreinterpret_u8_s16 (int16x4_t __a)
{
  return (uint8x8_t)__builtin_neon_vreinterpretv8qiv4hi (__a);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vreinterpret_u8_s32 (int32x2_t __a)
{
  return (uint8x8_t)__builtin_neon_vreinterpretv8qiv2si (__a);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vreinterpret_u8_s64 (int64x1_t __a)
{
  return (uint8x8_t)__builtin_neon_vreinterpretv8qidi (__a);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vreinterpret_u8_f32 (float32x2_t __a)
{
  return (uint8x8_t)__builtin_neon_vreinterpretv8qiv2sf (__a);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vreinterpret_u8_u16 (uint16x4_t __a)
{
  return (uint8x8_t)__builtin_neon_vreinterpretv8qiv4hi ((int16x4_t) __a);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vreinterpret_u8_u32 (uint32x2_t __a)
{
  return (uint8x8_t)__builtin_neon_vreinterpretv8qiv2si ((int32x2_t) __a);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vreinterpret_u8_u64 (uint64x1_t __a)
{
  return (uint8x8_t)__builtin_neon_vreinterpretv8qidi ((int64x1_t) __a);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vreinterpret_u8_p8 (poly8x8_t __a)
{
  return (uint8x8_t)__builtin_neon_vreinterpretv8qiv8qi ((int8x8_t) __a);
}

__extension__ static __inline uint8x8_t __attribute__ ((__always_inline__))
vreinterpret_u8_p16 (poly16x4_t __a)
{
  return (uint8x8_t)__builtin_neon_vreinterpretv8qiv4hi ((int16x4_t) __a);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vreinterpretq_u8_s8 (int8x16_t __a)
{
  return (uint8x16_t)__builtin_neon_vreinterpretv16qiv16qi (__a);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vreinterpretq_u8_s16 (int16x8_t __a)
{
  return (uint8x16_t)__builtin_neon_vreinterpretv16qiv8hi (__a);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vreinterpretq_u8_s32 (int32x4_t __a)
{
  return (uint8x16_t)__builtin_neon_vreinterpretv16qiv4si (__a);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vreinterpretq_u8_s64 (int64x2_t __a)
{
  return (uint8x16_t)__builtin_neon_vreinterpretv16qiv2di (__a);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vreinterpretq_u8_f32 (float32x4_t __a)
{
  return (uint8x16_t)__builtin_neon_vreinterpretv16qiv4sf (__a);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vreinterpretq_u8_u16 (uint16x8_t __a)
{
  return (uint8x16_t)__builtin_neon_vreinterpretv16qiv8hi ((int16x8_t) __a);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vreinterpretq_u8_u32 (uint32x4_t __a)
{
  return (uint8x16_t)__builtin_neon_vreinterpretv16qiv4si ((int32x4_t) __a);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vreinterpretq_u8_u64 (uint64x2_t __a)
{
  return (uint8x16_t)__builtin_neon_vreinterpretv16qiv2di ((int64x2_t) __a);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vreinterpretq_u8_p8 (poly8x16_t __a)
{
  return (uint8x16_t)__builtin_neon_vreinterpretv16qiv16qi ((int8x16_t) __a);
}

__extension__ static __inline uint8x16_t __attribute__ ((__always_inline__))
vreinterpretq_u8_p16 (poly16x8_t __a)
{
  return (uint8x16_t)__builtin_neon_vreinterpretv16qiv8hi ((int16x8_t) __a);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vreinterpret_u16_s8 (int8x8_t __a)
{
  return (uint16x4_t)__builtin_neon_vreinterpretv4hiv8qi (__a);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vreinterpret_u16_s16 (int16x4_t __a)
{
  return (uint16x4_t)__builtin_neon_vreinterpretv4hiv4hi (__a);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vreinterpret_u16_s32 (int32x2_t __a)
{
  return (uint16x4_t)__builtin_neon_vreinterpretv4hiv2si (__a);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vreinterpret_u16_s64 (int64x1_t __a)
{
  return (uint16x4_t)__builtin_neon_vreinterpretv4hidi (__a);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vreinterpret_u16_f32 (float32x2_t __a)
{
  return (uint16x4_t)__builtin_neon_vreinterpretv4hiv2sf (__a);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vreinterpret_u16_u8 (uint8x8_t __a)
{
  return (uint16x4_t)__builtin_neon_vreinterpretv4hiv8qi ((int8x8_t) __a);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vreinterpret_u16_u32 (uint32x2_t __a)
{
  return (uint16x4_t)__builtin_neon_vreinterpretv4hiv2si ((int32x2_t) __a);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vreinterpret_u16_u64 (uint64x1_t __a)
{
  return (uint16x4_t)__builtin_neon_vreinterpretv4hidi ((int64x1_t) __a);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vreinterpret_u16_p8 (poly8x8_t __a)
{
  return (uint16x4_t)__builtin_neon_vreinterpretv4hiv8qi ((int8x8_t) __a);
}

__extension__ static __inline uint16x4_t __attribute__ ((__always_inline__))
vreinterpret_u16_p16 (poly16x4_t __a)
{
  return (uint16x4_t)__builtin_neon_vreinterpretv4hiv4hi ((int16x4_t) __a);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vreinterpretq_u16_s8 (int8x16_t __a)
{
  return (uint16x8_t)__builtin_neon_vreinterpretv8hiv16qi (__a);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vreinterpretq_u16_s16 (int16x8_t __a)
{
  return (uint16x8_t)__builtin_neon_vreinterpretv8hiv8hi (__a);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vreinterpretq_u16_s32 (int32x4_t __a)
{
  return (uint16x8_t)__builtin_neon_vreinterpretv8hiv4si (__a);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vreinterpretq_u16_s64 (int64x2_t __a)
{
  return (uint16x8_t)__builtin_neon_vreinterpretv8hiv2di (__a);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vreinterpretq_u16_f32 (float32x4_t __a)
{
  return (uint16x8_t)__builtin_neon_vreinterpretv8hiv4sf (__a);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vreinterpretq_u16_u8 (uint8x16_t __a)
{
  return (uint16x8_t)__builtin_neon_vreinterpretv8hiv16qi ((int8x16_t) __a);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vreinterpretq_u16_u32 (uint32x4_t __a)
{
  return (uint16x8_t)__builtin_neon_vreinterpretv8hiv4si ((int32x4_t) __a);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vreinterpretq_u16_u64 (uint64x2_t __a)
{
  return (uint16x8_t)__builtin_neon_vreinterpretv8hiv2di ((int64x2_t) __a);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vreinterpretq_u16_p8 (poly8x16_t __a)
{
  return (uint16x8_t)__builtin_neon_vreinterpretv8hiv16qi ((int8x16_t) __a);
}

__extension__ static __inline uint16x8_t __attribute__ ((__always_inline__))
vreinterpretq_u16_p16 (poly16x8_t __a)
{
  return (uint16x8_t)__builtin_neon_vreinterpretv8hiv8hi ((int16x8_t) __a);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vreinterpret_u32_s8 (int8x8_t __a)
{
  return (uint32x2_t)__builtin_neon_vreinterpretv2siv8qi (__a);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vreinterpret_u32_s16 (int16x4_t __a)
{
  return (uint32x2_t)__builtin_neon_vreinterpretv2siv4hi (__a);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vreinterpret_u32_s32 (int32x2_t __a)
{
  return (uint32x2_t)__builtin_neon_vreinterpretv2siv2si (__a);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vreinterpret_u32_s64 (int64x1_t __a)
{
  return (uint32x2_t)__builtin_neon_vreinterpretv2sidi (__a);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vreinterpret_u32_f32 (float32x2_t __a)
{
  return (uint32x2_t)__builtin_neon_vreinterpretv2siv2sf (__a);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vreinterpret_u32_u8 (uint8x8_t __a)
{
  return (uint32x2_t)__builtin_neon_vreinterpretv2siv8qi ((int8x8_t) __a);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vreinterpret_u32_u16 (uint16x4_t __a)
{
  return (uint32x2_t)__builtin_neon_vreinterpretv2siv4hi ((int16x4_t) __a);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vreinterpret_u32_u64 (uint64x1_t __a)
{
  return (uint32x2_t)__builtin_neon_vreinterpretv2sidi ((int64x1_t) __a);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vreinterpret_u32_p8 (poly8x8_t __a)
{
  return (uint32x2_t)__builtin_neon_vreinterpretv2siv8qi ((int8x8_t) __a);
}

__extension__ static __inline uint32x2_t __attribute__ ((__always_inline__))
vreinterpret_u32_p16 (poly16x4_t __a)
{
  return (uint32x2_t)__builtin_neon_vreinterpretv2siv4hi ((int16x4_t) __a);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vreinterpretq_u32_s8 (int8x16_t __a)
{
  return (uint32x4_t)__builtin_neon_vreinterpretv4siv16qi (__a);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vreinterpretq_u32_s16 (int16x8_t __a)
{
  return (uint32x4_t)__builtin_neon_vreinterpretv4siv8hi (__a);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vreinterpretq_u32_s32 (int32x4_t __a)
{
  return (uint32x4_t)__builtin_neon_vreinterpretv4siv4si (__a);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vreinterpretq_u32_s64 (int64x2_t __a)
{
  return (uint32x4_t)__builtin_neon_vreinterpretv4siv2di (__a);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vreinterpretq_u32_f32 (float32x4_t __a)
{
  return (uint32x4_t)__builtin_neon_vreinterpretv4siv4sf (__a);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vreinterpretq_u32_u8 (uint8x16_t __a)
{
  return (uint32x4_t)__builtin_neon_vreinterpretv4siv16qi ((int8x16_t) __a);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vreinterpretq_u32_u16 (uint16x8_t __a)
{
  return (uint32x4_t)__builtin_neon_vreinterpretv4siv8hi ((int16x8_t) __a);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vreinterpretq_u32_u64 (uint64x2_t __a)
{
  return (uint32x4_t)__builtin_neon_vreinterpretv4siv2di ((int64x2_t) __a);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vreinterpretq_u32_p8 (poly8x16_t __a)
{
  return (uint32x4_t)__builtin_neon_vreinterpretv4siv16qi ((int8x16_t) __a);
}

__extension__ static __inline uint32x4_t __attribute__ ((__always_inline__))
vreinterpretq_u32_p16 (poly16x8_t __a)
{
  return (uint32x4_t)__builtin_neon_vreinterpretv4siv8hi ((int16x8_t) __a);
}

#ifdef __cplusplus
}
#endif
#endif
#endif

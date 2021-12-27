#ifndef _JNIX_KERNEL_H
#define _JNIX_KERNEL_H

#include <jnix/types.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))

#define PTR_IF(cond, ptr)	((cond) ? (ptr) : NULL)

#define upper_32_bits(n) ((u32)(((n) >> 16) >> 16))
#define lower_32_bits(n) ((u32)((n) & 0xffffffff))
#define upper_16_bits(n) ((u16)((n) >> 16))
#define lower_16_bits(n) ((u16)((n) & 0xffff))

#endif
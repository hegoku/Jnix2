#include <asm/desc.h>
#include <asm/irq_vectors.h>

#define BITS_PER_BYTE 8
#define BITS_PER_TYPE(type)	(sizeof(type) * BITS_PER_BYTE)
#define __KERNEL_DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define BITS_TO_LONGS(nr) __KERNEL_DIV_ROUND_UP(nr, BITS_PER_TYPE(u64))
#define DECLARE_BITMAP(name,bits) \
	unsigned long name[BITS_TO_LONGS(bits)]

DECLARE_BITMAP(system_vectors, NR_VECTORS);
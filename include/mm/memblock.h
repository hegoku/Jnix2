#ifndef MM_MEMBLOCK_H
#define MM_MEMBLOCK_H

#include <jnix/init.h>
#include <jnix/types.h>

#define __init_memblock __meminit
#define __initdata_memblock __meminitdata

#define MEMBLOCK_ALLOC_ANYWHERE	(~(phys_addr_t)0)
#define MEMBLOCK_ALLOC_ACCESSIBLE	0
#define MEMBLOCK_ALLOC_NOLEAKTRACE	1

#define MEMBLOCK_LOW_LIMIT 0

enum memblock_flags {
	MEMBLOCK_NONE		= 0x0,	/* No special request */
	MEMBLOCK_HOTPLUG	= 0x1,	/* hotpluggable region */
	MEMBLOCK_MIRROR		= 0x2,	/* mirrored region */
	MEMBLOCK_NOMAP		= 0x4,	/* don't add to kernel direct mapping */
	MEMBLOCK_DRIVER_MANAGED = 0x8,	/* always detected via a driver */
};

struct memblock_region {
	phys_addr_t base;
	phys_addr_t size;
	enum memblock_flags flags;
};

struct memblock_type {
	unsigned long cnt;	/* number of regions */
	unsigned long max;	/* size of the allocated array */
	phys_addr_t total_size;	/* size of all regions */
	struct memblock_region *regions;
	char *name;
};

struct memblock {
	int bottom_up;  /* is bottom up direction? */
	phys_addr_t current_limit;
	struct memblock_type memory;
	struct memblock_type reserved;
};

extern struct memblock memblock;

static inline __init_memblock int memblock_bottom_up(void)
{
	return memblock.bottom_up;
}

static inline void memblock_set_region_node(struct memblock_region *r, int nid)
{
}

static inline int memblock_get_region_node(const struct memblock_region *r)
{
	return 0;
}

void __next_mem_range(u64 *idx, int nid, enum memblock_flags flags,
		      struct memblock_type *type_a,
		      struct memblock_type *type_b, phys_addr_t *out_start,
		      phys_addr_t *out_end, int *out_nid);

void __next_mem_range_rev(u64 *idx, int nid, enum memblock_flags flags,
			  struct memblock_type *type_a,
			  struct memblock_type *type_b, phys_addr_t *out_start,
			  phys_addr_t *out_end, int *out_nid);

#define __for_each_mem_range(i, type_a, type_b, nid, flags,		\
			   p_start, p_end, p_nid)			\
	for (i = 0, __next_mem_range(&i, nid, flags, type_a, type_b,	\
				     p_start, p_end, p_nid);		\
	     i != (u64)ULLONG_MAX;					\
	     __next_mem_range(&i, nid, flags, type_a, type_b,		\
			      p_start, p_end, p_nid))

#define __for_each_mem_range_rev(i, type_a, type_b, nid, flags,		\
				 p_start, p_end, p_nid)			\
	for (i = (u64)ULLONG_MAX,					\
		     __next_mem_range_rev(&i, nid, flags, type_a, type_b, \
					  p_start, p_end, p_nid);	\
	     i != (u64)ULLONG_MAX;					\
	     __next_mem_range_rev(&i, nid, flags, type_a, type_b,	\
				  p_start, p_end, p_nid))

#define for_each_free_mem_range(i, nid, flags, p_start, p_end, p_nid)	\
	__for_each_mem_range(i, &memblock.memory, &memblock.reserved,	\
			     nid, flags, p_start, p_end, p_nid)

#define for_each_free_mem_range_reverse(i, nid, flags, p_start, p_end,	\
					p_nid)				\
	__for_each_mem_range_rev(i, &memblock.memory, &memblock.reserved, \
				 nid, flags, p_start, p_end, p_nid)

int memblock_add_node(phys_addr_t base, phys_addr_t size, int nid,
		      enum memblock_flags flags);
int memblock_add(phys_addr_t base, phys_addr_t size);
int memblock_remove(phys_addr_t base, phys_addr_t size);
int memblock_phys_free(phys_addr_t base, phys_addr_t size);
int memblock_reserve(phys_addr_t base, phys_addr_t size);

void memblock_print();
#endif
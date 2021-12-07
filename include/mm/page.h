#ifndef MM_PAGE_H
#define MM_PAGE_H

#include <jnix/types.h>

struct page {
	phys_addr_t address;
	int flag;
};

extern struct page *mem_map;

#endif
#include <asm/page.h>
#include <jnix/init.h>
#include <mm/page.h>
#include <mm/memblock.h>
#include <mm/fpn.h>
#include <jnix/printk.h>

void __init init_page()
{
	int i = 0;
	unsigned long mapped_pfn = max_pfn_mapped/PTRS_PER_PGD;
	for (i=0;i<mapped_pfn;i++) {
		swapper_pg_dir[i].pgd = 0;
	}

	mem_map = __va(memblock_alloc_range_nid(max_pfn*sizeof(struct page), PAGE_SIZE, MEMBLOCK_LOW_LIMIT, MEMBLOCK_ALLOC_ACCESSIBLE, 0, 0));
	for (i=0;i<max_pfn;i++) {
		mem_map[i].address = PFN_PHYS(i);
	}
	printk("mem_map:%x\n", mem_map);
}
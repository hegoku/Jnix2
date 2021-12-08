#include <mm/page.h>
#include <mm/memblock.h>
#include <mm/fpn.h>
#include <mm/mm.h>
#include <jnix/printk.h>

void __init init_mm()
{
	u64 i;
	mem_map = __va(memblock_alloc_range_nid(max_pfn*sizeof(struct page), PAGE_SIZE, MEMBLOCK_LOW_LIMIT, MEMBLOCK_ALLOC_ACCESSIBLE, 0, 0));
	for (i=0;i<max_pfn;i++) {
		mem_map[i].order  = -1;
		mem_map[i].size = 0;
	}
	printk("mem_map:%x\n", mem_map);

	buddy_init();
	phys_addr_t this_start, this_end, cand;
	u64 j = 0;
	for_each_free_mem_range(j, 0, MEMBLOCK_NONE, &this_start, &this_end, NULL) {
		unsigned long pfn = PFN_UP(this_start);
		// unsigned long e_pfn = clamp(PFN_DOWN(this_end), 0, max_pfn);
		unsigned long e_pfn = min_t(unsigned long, PFN_DOWN(this_end), max_pfn);
		for (;pfn<e_pfn;pfn++) {
			__free_one_page(&mem_map[pfn], 0);
		}
	}
	buddy_print();
}
#include <asm/page.h>
#include <jnix/types.h>
#include <mm/page.h>
#include <jnix/printk.h>
#include <mm/fpn.h>

struct page *mem_map;

struct free_area free_area[MAX_ORDER];

static inline unsigned long __find_buddy_index(unsigned long page_idx, unsigned int order)
{
	return page_idx ^ (1 << order);
}

static inline int get_order(unsigned int size)
{
	int i = 0;
	while(i<32) {
		if (size&(1<<i))
			return i;
		i++;
	}
	return -1;
}

static inline int fix_size(unsigned int size)
{
	size -=1;
	size |= size>>1;
	size |= size>>2;
	size |= size>>4;
	size |= size>>6;
	size |= size>>16;
	return size+1;
}

static inline int page_is_buddy(struct page *buddy, int order)
{
	return buddy->order == order;
}

void __free_one_page(struct page *page, unsigned int order)
{
	unsigned long buddy_idx,combined_idx,page_idx;
	struct page *buddy;

	int pfn = PAGE_TO_FPN(page);
	page->size = 0;
	page_idx = pfn & ((1<<MAX_ORDER)-1);
	while (order < MAX_ORDER - 1) {
		buddy_idx = __find_buddy_index(page_idx, order);
		buddy = page + (buddy_idx - page_idx);
		if (!page_is_buddy(buddy, order))
			break;
		__list_del_entry(&buddy->page_link);
		free_area[order].nr_free--;
		buddy->order = -1;
		buddy->size = 0;
		combined_idx = buddy_idx & page_idx;
		page = page + (combined_idx - page_idx);
		page_idx = combined_idx;
		order++;
	}
	page->order = order;
	page->size = 1<<order;
	list_add(&(page->page_link), &(free_area[order].free_list));
	free_area[order].nr_free++;
}

void __free_page(struct page *page)
{
	int order = get_order(page->size);
	__free_one_page(page, order);
}

void free_page(phys_addr_t addr)
{
	struct page *page = &mem_map[PHYS_PFN(__pa(addr))];
	__free_page(page);
}

phys_addr_t get_free_page()
{
	return (phys_addr_t)__va((alloc_pages(1)-mem_map)<<PAGE_SHIFT);
}

void expand(unsigned int low, unsigned int high, struct page *page, struct free_area *area)
{
	unsigned int size;
	while(low<high) {
		high--;
		area--;
		size = 1<<high;
		list_add(&page[size].page_link, &area->free_list);
		area->nr_free++;
		page[size].order = high;
		page[size].size = size;
	}
}

static struct page *__alloc_pages_by_order(unsigned int order)
{
	unsigned int current_order;
	struct free_area *area;
	struct page *page;
	for(current_order=order;current_order<MAX_ORDER;current_order++) {
		area = &(free_area[current_order]);
		page = list_first_entry_or_null(&area->free_list, struct page, page_link);
		if (!page)
			continue;
		__list_del_entry(&(page->page_link));
		page->order = -1;
		page->size = 1<<order;
		area->nr_free--;
		expand(order, current_order, page, area);
		return page;
	}
	return NULL;
}

struct page *alloc_pages(unsigned int size)
{
	struct page *page;
	unsigned int alloc_size = fix_size(size);
	unsigned int order = get_order(alloc_size);
	if (order<0) {
		printk("alloc_page order<0: %d\n", order);
		return NULL;
	}
	if (order>MAX_ORDER) {
		printk("alloc_page order>MAX_ORDER: %d\n", order);
		return NULL;
	}
	page = __alloc_pages_by_order(order);
	if (page)
		return page;
	return NULL;
}

void buddy_init()
{
	int i = 0;
	for(;i<MAX_ORDER;i++)
		INIT_LIST_HEAD(&(free_area[i].free_list));
}

void buddy_print()
{
	printk("buddy-order:");
	for (int i=0;i<MAX_ORDER;i++) {
		printk("  %#02d", i);
	}

	printk("\nbuddy-count:");
	for (int i=0;i<MAX_ORDER;i++) {
		printk("  %#02d", free_area[i].nr_free);
	}
	printk("\n");
}
#ifndef MM_PAGE_H
#define MM_PAGE_H

#include <jnix/types.h>
#include <jnix/list.h>
#include <asm/page.h>

struct page {
	struct list_head page_link;
	int flag;
	int size;
	unsigned short order;
};

struct free_area {
	struct list_head free_list;
	u32 nr_free;
};

#define MAX_ORDER 11
#define PAGE_TO_FPN(page) ((page-mem_map))

extern struct page *mem_map;

struct page *alloc_pages(unsigned int size);
void __free_one_page(struct page *page, unsigned int order);
void __free_page(struct page *page);
void free_page(phys_addr_t addr);
void buddy_print();
void buddy_init();

phys_addr_t get_free_page();
#endif
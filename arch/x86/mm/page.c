#include <asm/page.h>
#include <jnix/init.h>
#include <mm/page.h>
#include <jnix/printk.h>
#include <jnix/init.h>

void __init init_page()
{
	int i = 0;
	unsigned long mapped_pfn = max_pfn_mapped/PTRS_PER_PGD;
	for (i=0;i<mapped_pfn;i++) {
		swapper_pg_dir[i].pgd = 0;
	}
}
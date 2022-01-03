#include <asm/page.h>
#include <jnix/init.h>
#include <mm/page.h>
#include <jnix/printk.h>
#include <jnix/init.h>
#include <mm/kmalloc.h>
#include <jnix/mm_types.h>
#include <asm/pgalloc.h>

void __init init_page()
{
	int i = 0;
	unsigned long mapped_pfn = max_pfn_mapped/PTRS_PER_PGD;
	for (i=0;i<mapped_pfn;i++) {
		swapper_pg_dir[i].pgd = 0;
	}
}

pgd_t *pgd_alloc(struct mm_struct *mm)
{
	/*
	 * If no SHARED_KERNEL_PMD, PAE kernel is running as a Xen domain.
	 * We allocate one page for pgd.
	 */
	// if (!SHARED_KERNEL_PMD)
	// 	return (pgd_t *)__get_free_pages(GFP_PGTABLE_USER,
	// 					 PGD_ALLOCATION_ORDER);

	/*
	 * Now PAE kernel is not running as a Xen domain. We can allocate
	 * a 32-byte slab for pgd to save memory space.
	 */
	return (pgd_t*)get_free_page();
}
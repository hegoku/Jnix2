#include <jnix/init.h>
#include <asm/page.h>
#include <asm/setup.h>
#include <asm/sections.h>
#include <jnix/start_kernel.h>

__attribute__((regparm(0))) void __init i386_start_kernel(void)
{
	/* Make sure IDT is set up before any exception happens */
	idt_setup_early_handler();

	// cr4_init_shadow();

	// sanitize_boot_params(&boot_params);

	// x86_early_init_platform_quirks();

	/* Call the subarch specific early setup function */
	// switch (boot_params.hdr.hardware_subarch) {
	// case X86_SUBARCH_INTEL_MID:
	// 	x86_intel_mid_early_setup();
	// 	break;
	// case X86_SUBARCH_CE4100:
	// 	x86_ce4100_early_setup();
	// 	break;
	// default:
	// 	i386_default_early_setup();
	// 	break;
	// }

	start_kernel();
}

/*
 * Initialize page tables.  This creates a PDE and a set of page
 * tables, which are located immediately beyond __brk_base.  The variable
 * _brk_end is set up to point to the first "safe" location.
 * Mappings are created both at virtual address 0 (identity mapping)
 * and PAGE_OFFSET for up to _end.
 *
 * In PAE mode initial_page_table is statically defined to contain
 * enough entries to cover the VMSPLIT option (that is the top 1, 2 or 3
 * entries). The identity mapping is handled by pointing two PGD entries
 * to the first kernel PMD. Note the upper half of each PMD or PTE are
 * always zero at this stage.
 */
void __init mk_early_pgtbl_32(void)
{
	pte_t pte, *ptep;
	int i;
	unsigned long *ptr;
	/* Enough space to fit pagetables for the low memory linear map */
	// const unsigned long limit = __pa(_end) +
		// (PAGE_TABLE_SIZE(LOWMEM_PAGES) << PAGE_SHIFT);
	const unsigned long limit = __pa(_end);
// #ifdef CONFIG_X86_PAE
// 	pmd_t pl2, *pl2p = (pmd_t *)__pa(initial_pg_pmd);
// #define SET_PL2(pl2, val)    { (pl2).pmd = (val); }
// #else
	pgd_t pl2, *pl2p = (pgd_t *)__pa(swapper_pg_dir);
#define SET_PL2(pl2, val)   { (pl2).pgd = (val); }
// #endif

	ptep = (pte_t *)__pa(__brk_base);
	pte.pte = PTE_IDENT_ATTR;

	while ((pte.pte & PTE_PFN_MASK) < limit) {

		SET_PL2(pl2, (unsigned long)ptep | PDE_IDENT_ATTR);
		*pl2p = pl2;
		// *(pl2p +  ((PAGE_OFFSET >> PGDIR_SHIFT))) = pl2;
		*(pl2p +  768) = pl2;
		for (i = 0; i < 1024; i++) {
			*ptep = pte;
			pte.pte += PAGE_SIZE;
			ptep++;
		}

		pl2p++;
		break;
	}

	ptr = (unsigned long *)__pa(&max_pfn_mapped);
	/* Can't use pte_pfn() since it's a call with CONFIG_PARAVIRT */
	*ptr = (pte.pte & PTE_PFN_MASK) >> PAGE_SHIFT;

	ptr = (unsigned long *)__pa(&_brk_end);
	*ptr = (unsigned long)ptep + PAGE_OFFSET;
}
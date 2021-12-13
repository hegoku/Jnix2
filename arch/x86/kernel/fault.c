#include <asm/ptrace.h>
#include <asm/fault.h>
#include <jnix/printk.h>
#include <mm/page.h>

#define __FORCE_ORDER "m"(*(unsigned int *)0x1000UL)

#define invalidate() \
__asm__ __volatile__("movl %%cr3,%%eax\n\tmovl %%eax,%%cr3": : :"ax")

static inline unsigned long read_cr2(void)
{
	unsigned long val;
	asm volatile("mov %%cr2,%0\n\t" : "=r" (val) : __FORCE_ORDER);
	return val;
}

// noinstr void exc_page_fault(struct pt_regs *regs, unsigned long error_code)
__attribute__((regparm(3))) void exc_page_fault(struct pt_regs *regs, unsigned long error_code)
{
	unsigned long address = read_cr2();
	printk("page_fault:%x %x\n", address, error_code);

	int index1 = address >> 22;
    int index2 = address >> 12 & 0x03FF;
	printk("%d, %d\n", index1, index2);

	phys_addr_t page = get_free_page();
	phys_addr_t pte = swapper_pg_dir[index1].pgd & PTE_PFN_MASK;
	if (pte == NULL) {
		pte = get_free_page();
		swapper_pg_dir[index1].pgd = __pa(pte)|PDE_IDENT_ATTR;
	}
	phys_addr_t *ptep = pte;
	ptep[index2] = __pa(page)|PTE_IDENT_ATTR;
	printk("%x %x\n",swapper_pg_dir, swapper_pg_dir[index1].pgd);
	invalidate();
}
#include <asm/ptrace.h>
#include <asm/fault.h>
#include <jnix/printk.h>
#include <mm/page.h>
#include <asm/current.h>

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
	printk("pid:%d page_fault: address:%#x error_code:%#x\n", current->pid, address, error_code);
	if (error_code&1) {
		printk("page-protection violation\n");
		for (;;)
		{
		}
	}

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

__attribute__((regparm(3))) void exc_de(struct pt_regs *regs, unsigned long error_code)
{
	printk("de\n");
	for (;;)
	{
	}
}

__attribute__((regparm(3))) void exc_db(struct pt_regs *regs, unsigned long error_code)
{
	printk("db\n");
	for (;;)
	{
	}
}

__attribute__((regparm(3))) void exc_nmi(struct pt_regs *regs, unsigned long error_code)
{
	printk("nmi\n");
	for (;;)
	{
	}
}

__attribute__((regparm(3))) void exc_bp(struct pt_regs *regs, unsigned long error_code)
{
	printk("bp\n");
	for (;;)
	{
	}
}

__attribute__((regparm(3))) void exc_of(struct pt_regs *regs, unsigned long error_code)
{
	printk("of\n");
	for (;;)
	{
	}
}

__attribute__((regparm(3))) void exc_ts(struct pt_regs *regs, unsigned long error_code)
{
	printk("ts\n");
	for (;;)
	{
	}
}

__attribute__((regparm(3))) void exc_np(struct pt_regs *regs, unsigned long error_code)
{
	printk("np\n");
	for (;;)
	{
	}
}
__attribute__((regparm(3))) void exc_ss(struct pt_regs *regs, unsigned long error_code)
{
	printk("ss\n");
	for (;;)
	{
	}
}

__attribute__((regparm(3))) void exc_gp(struct pt_regs *regs, unsigned long error_code)
{
	unsigned char index, tbl, e;
	index = error_code >> 3;
	tbl = (error_code&0x6) >> 1;
	e = error_code & 0x1;
	printk("General Protection: code:%d e:%d ", error_code, e);
	if ((tbl & 0x1) || (tbl & 0x3))
	{
		printk("IDT:%#x ", index);
	}
	else if (tbl & 0x0 == 0)
	{
		printk("GDT:%#x ", index);
	}
	else if (tbl & 0x2)
	{
		printk("LDT:%#x ", index);
	}
	printk("pid:%d thread_sp:%#x\n", current->pid, current->thread.sp0);
	printk("ss:%#x sp:%#x flags:%#x cs:%#x ip:%#x\n", regs->ss, regs->sp, regs->flags, regs->cs, regs->ip);
	for (;;)
	{
	}
}
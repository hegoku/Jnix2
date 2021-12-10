#include <asm/ptrace.h>
#include <asm/fault.h>
#include <jnix/printk.h>
#include <mm/page.h>

#define __FORCE_ORDER "m"(*(unsigned int *)0x1000UL)

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
	for(;;){}

}
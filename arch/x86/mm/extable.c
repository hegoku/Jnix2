#include <jnix/init.h>
#include <asm/ptrace.h>
#include <asm/extable.h>
#include <asm/traps.h>
#include <asm/segment.h>

extern unsigned int early_recursion_flag;

void __attribute__((regparm(3))) __init early_fixup_exception(struct pt_regs *regs, int trapnr) 
{
	/* Ignore early NMIs. */
	if (trapnr == X86_TRAP_NMI)
		return;

	if (early_recursion_flag > 2){
		for (;;)
			{
				asm("hlt;");
			}
	}

	if (regs->cs != __KERNEL_CS) {
		for (;;)
			{
				asm("hlt;");
			}
	}
	/*
	 * The full exception fixup machinery is available as soon as
	 * the early IDT is loaded.  This means that it is the
	 * responsibility of extable users to either function correctly
	 * when handlers are invoked early or to simply avoid causing
	 * exceptions before they're ready to handle them.
	 *
	 * This is better than filtering which handlers can be used,
	 * because refusing to call a handler here is guaranteed to
	 * result in a hard-to-debug panic.
	 *
	 * Keep in mind that not all vectors actually get here.  Early
	 * page faults, for example, are special.
	 */
	asm("movl %0, %%ebx" ::"r"(regs->orig_ax));
	// if (fixup_exception(regs, trapnr, regs->orig_ax, 0))
	// 	return;

	// if (trapnr == X86_TRAP_UD) {
	// 	if (report_bug(regs->ip, regs) == BUG_TRAP_TYPE_WARN) {
	// 		/* Skip the ud2. */
	// 		regs->ip += LEN_UD2;
	// 		return;
	// 	}

		/*
		 * If this was a BUG and report_bug returns or if this
		 * was just a normal #UD, we want to continue onward and
		 * crash.
		 */
	// }

// fail:
// 	early_printk("PANIC: early exception 0x%02x IP %lx:%lx error %lx cr2 0x%lx\n",
// 		     (unsigned)trapnr, (unsigned long)regs->cs, regs->ip,
// 		     regs->orig_ax, read_cr2());

// 	show_regs(regs);

	for (;;)
	{
		asm("hlt;");
	}
}
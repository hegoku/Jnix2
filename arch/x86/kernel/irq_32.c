#include <asm/irq.h>
#include <jnix/irqdesc.h>
#include <asm/ptrace.h>

// static int check_stack_overflow(void)
// {
// 	long sp;

// 	__asm__ __volatile__("andl %%esp,%0" :
// 			     "=r" (sp) : "0" (THREAD_SIZE - 1));

// 	return sp < (sizeof(struct thread_info) + STACK_WARN);
// }


void __handle_irq(struct irq_desc *desc, struct pt_regs *regs)
{
	// int overflow = check_stack_overflow();

	// if (user_mode(regs) || !execute_on_irq_stack(overflow, desc)) {
	if (user_mode(regs)) {
		// if (unlikely(overflow))
			// print_stack_overflow();
		generic_handle_irq_desc(desc);
	}
}
#include <asm/irq.h>
#include <jnix/irqdesc.h>
#include <asm/ptrace.h>

static int check_stack_overflow(void)
{
// 	long sp;

// 	__asm__ __volatile__("andl %%esp,%0" :
// 			     "=r" (sp) : "0" (THREAD_SIZE - 1));

// 	return sp < (sizeof(struct thread_info) + STACK_WARN);
	return 0;
}

static inline int execute_on_irq_stack(int overflow, struct irq_desc *desc)
{
	return 0;
	// struct irq_stack *curstk, *irqstk;
	// u32 *isp, *prev_esp, arg1;

	// curstk = (struct irq_stack *) current_stack();
	// irqstk = __this_cpu_read(hardirq_stack_ptr);

	// /*
	//  * this is where we switch to the IRQ stack. However, if we are
	//  * already using the IRQ stack (because we interrupted a hardirq
	//  * handler) we can't do that and just have to keep using the
	//  * current stack (which is the irq stack already after all)
	//  */
	// if (unlikely(curstk == irqstk))
	// 	return 0;

	// isp = (u32 *) ((char *)irqstk + sizeof(*irqstk));

	// /* Save the next esp at the bottom of the stack */
	// prev_esp = (u32 *)irqstk;
	// *prev_esp = current_stack_pointer;

	// if (unlikely(overflow))
	// 	call_on_stack(print_stack_overflow, isp);

	// asm volatile("xchgl	%%ebx,%%esp	\n"
	// 	     CALL_NOSPEC
	// 	     "movl	%%ebx,%%esp	\n"
	// 	     : "=a" (arg1), "=b" (isp)
	// 	     :  "0" (desc),   "1" (isp),
	// 		[thunk_target] "D" (desc->handle_irq)
	// 	     : "memory", "cc", "ecx");
	// return 1;
}

void __handle_irq(struct irq_desc *desc, struct pt_regs *regs)
{
	int overflow = check_stack_overflow();

	if (user_mode(regs) || !execute_on_irq_stack(overflow, desc)) {
		// if (unlikely(overflow))
			// print_stack_overflow();
		generic_handle_irq_desc(desc);
	}
}
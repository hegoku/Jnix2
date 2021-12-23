#include <jnix/irqdesc.h>
#include <asm/hw_irq.h>
#include <jnix/printk.h>

struct pt_regs * __irq_regs;

static inline struct pt_regs *set_irq_regs(struct pt_regs *new_regs)
{
	struct pt_regs *old_regs;

	old_regs = __irq_regs;
	__irq_regs = new_regs;
	return old_regs;
}

static inline void handle_irq(struct irq_desc *desc,
				       struct pt_regs *regs)
{
	// if (IS_ENABLED(CONFIG_X86_64))
		// generic_handle_irq_desc(desc);
	// else
		__handle_irq(desc, regs);
}

__attribute__((regparm(3))) void common_interrupt(struct pt_regs *regs, u32 vector)
{
	struct pt_regs *old_regs = set_irq_regs(regs);
	struct irq_desc *desc;

	/* entry code tells RCU that we're not quiescent.  Check it. */
	// RCU_LOCKDEP_WARN(!rcu_is_watching(), "IRQ failed to wake up RCU");

	desc = vector_irq[vector];
	// if (likely(!IS_ERR_OR_NULL(desc))) {
	if (desc!=NULL) {
		handle_irq(desc, regs);
	} else {
		// ack_APIC_irq();

		if (desc == VECTOR_UNUSED) {
			printk("%s: %u No irq handler for vector\n", __func__, vector);
		} else {
			vector_irq[vector] = VECTOR_UNUSED;
		}
	}

	set_irq_regs(old_regs);
}
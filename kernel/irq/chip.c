#include <jnix/irqdesc.h>
#include <asm/irq.h>
#include <jnix/irq.h>

#include "internals.h"

static inline void mask_ack_irq(struct irq_desc *desc)
{
	if (desc->irq_data.chip->irq_mask_ack) {
		desc->irq_data.chip->irq_mask_ack(&desc->irq_data);
		// irq_state_set_masked(desc);
	} else {
		mask_irq(desc);
		if (desc->irq_data.chip->irq_ack)
			desc->irq_data.chip->irq_ack(&desc->irq_data);
	}
}

int irq_set_chip(unsigned int irq, struct irq_chip *chip)
{
	irq_desc[irq]->irq_data.chip = chip;
	/*
	 * For !CONFIG_SPARSE_IRQ make the irq show up in
	 * allocated_irqs.
	 */
	// irq_mark_irq(irq);
	return 0;
}

void mask_irq(struct irq_desc *desc)
{
	// if (irqd_irq_masked(&desc->irq_data))
	// 	return;

	if (desc->irq_data.chip->irq_mask) {
		desc->irq_data.chip->irq_mask(&desc->irq_data);
		// irq_state_set_masked(desc);
	}
}

void unmask_irq(struct irq_desc *desc)
{
	// if (!irqd_irq_masked(&desc->irq_data))
	// 	return;

	if (desc->irq_data.chip->irq_unmask) {
		desc->irq_data.chip->irq_unmask(&desc->irq_data);
		// irq_state_clr_masked(desc);
	}
}

void handle_level_irq(struct irq_desc *desc)
{
	mask_ack_irq(desc);

	// // if (!irq_may_run(desc))
	// 	// goto out_unlock;

	// // desc->istate &= ~(IRQS_REPLAY | IRQS_WAITING);

	// /*
	//  * If its disabled or no action available
	//  * keep it masked and get out of here
	//  */
	// // if (unlikely(!desc->action || irqd_irq_disabled(&desc->irq_data))) {
	// // 	desc->istate |= IRQS_PENDING;
	// // 	goto out_unlock;
	// // }
	if (!desc->action) {
		return;
	}

	// // kstat_incr_irqs_this_cpu(desc);
	handle_irq_event(desc);

	// cond_unmask_irq(desc);

// out_unlock:
	// raw_spin_unlock(&desc->lock);
}

static void __irq_do_set_handler(struct irq_desc *desc, irq_flow_handler_t handle,
		     int is_chained, const char *name)
{
	if (!handle) {
		handle = handle_bad_irq;
	}
	if (handle == handle_bad_irq) {
		if (desc->irq_data.chip != &no_irq_chip)
			mask_ack_irq(desc);
		// irq_state_set_disabled(desc);
		if (is_chained)
			desc->action = NULL;
		desc->depth = 1;
	}
	desc->handle_irq = handle;
	desc->name = name;
}
void __irq_set_handler(unsigned int irq, irq_flow_handler_t handle, int is_chained, const char *name)
{
	__irq_do_set_handler(irq_desc[irq], handle, is_chained, name);
}

void irq_set_chip_and_handler_name(unsigned int irq, struct irq_chip *chip,
			      irq_flow_handler_t handle, const char *name)
{
	irq_set_chip(irq, chip);
	__irq_set_handler(irq, handle, 0, name);
}
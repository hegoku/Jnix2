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

static void cond_unmask_irq(struct irq_desc *desc)
{
	/*
	 * We need to unmask in the following cases:
	 * - Standard level irq (IRQF_ONESHOT is not set)
	 * - Oneshot irq which did not wake the thread (caused by a
	 *   spurious interrupt or a primary handler handling it
	 *   completely).
	 */
	// if (!irqd_irq_disabled(&desc->irq_data) &&
	    // irqd_irq_masked(&desc->irq_data) && !desc->threads_oneshot)
		unmask_irq(desc);
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

	cond_unmask_irq(desc);

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

static int __irq_startup(struct irq_desc *desc)
{
	struct irq_data *d = irq_desc_get_irq_data(desc);
	int ret = 0;
	/* Warn if this interrupt is not activated but try nevertheless */
	// WARN_ON_ONCE(!irqd_is_activated(d));

	if (d->chip->irq_startup) {
		ret = d->chip->irq_startup(d);
		// irq_state_clr_disabled(desc);
		// irq_state_clr_masked(desc);
	} else {
		irq_enable(desc);
	}
	// irq_state_set_started(desc);
	return ret;
}

int irq_startup(struct irq_desc *desc, int resend, int force)
{
	struct irq_data *d = irq_desc_get_irq_data(desc);
	// struct cpumask *aff = irq_data_get_affinity_mask(d);
	int ret = 0;

	// desc->depth = 0;
	return __irq_startup(desc);
	// if (irqd_is_started(d)) {
	// 	irq_enable(desc);
	// } else {
	// 	switch (__irq_startup_managed(desc, aff, force)) {
	// 	case IRQ_STARTUP_NORMAL:
	// 		if (d->chip->flags & IRQCHIP_AFFINITY_PRE_STARTUP)
	// 			irq_setup_affinity(desc);
	// 		ret = __irq_startup(desc);
	// 		if (!(d->chip->flags & IRQCHIP_AFFINITY_PRE_STARTUP))
	// 			irq_setup_affinity(desc);
	// 		break;
	// 	case IRQ_STARTUP_MANAGED:
	// 		irq_do_set_affinity(d, aff, false);
	// 		ret = __irq_startup(desc);
	// 		break;
	// 	case IRQ_STARTUP_ABORT:
	// 		irqd_set_managed_shutdown(d);
	// 		return 0;
	// 	}
	// }
	// if (resend)
	// 	check_irq_resend(desc, false);

	return ret;
}

void irq_enable(struct irq_desc *desc)
{
	// if (!irqd_irq_disabled(&desc->irq_data)) {
	// 	unmask_irq(desc);
	// } else {
	// 	irq_state_clr_disabled(desc);
		if (desc->irq_data.chip->irq_enable) {
			desc->irq_data.chip->irq_enable(&desc->irq_data);
			// irq_state_clr_masked(desc);
		} else {
			unmask_irq(desc);
		}
	// }
}

static void __irq_disable(struct irq_desc *desc, int mask)
{
	// if (irqd_irq_disabled(&desc->irq_data)) {
	// 	if (mask)
	// 		mask_irq(desc);
	// } else {
		// irq_state_set_disabled(desc);
		if (desc->irq_data.chip->irq_disable) {
			desc->irq_data.chip->irq_disable(&desc->irq_data);
			// irq_state_set_masked(desc);
		} else if (mask) {
			mask_irq(desc);
		}
	// }
}

void irq_disable(struct irq_desc *desc)
{
	// __irq_disable(desc, irq_settings_disable_unlazy(desc));
	__irq_disable(desc, 0);
}

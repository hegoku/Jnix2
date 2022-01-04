#include <jnix/irq.h>
#include <jnix/interrupt.h>
#include <jnix/irqnr.h>
#include <mm/kmalloc.h>
#include <jnix/printk.h>
#include <uapi/asm-generic/errno.h>

#include "internals.h"

static int __setup_irq(unsigned int irq, struct irq_desc *desc, struct irqaction *new)
{
	struct irqaction *old, **old_ptr;

	if (!desc)
		return -1;
	if (desc->irq_data.chip == &no_irq_chip)
		return -1;

	new->irq = irq;

	old_ptr = &desc->action;
	old = *old_ptr;
	if (old) {
		do {
			old_ptr = &old->next;
			old = *old_ptr;
		} while (old);
	}

	*old_ptr = new;

	desc->irq_count = 0;
	desc->irqs_unhandled = 0;

	__enable_irq(desc);

	return 0;
}

int request_irq(unsigned int irq, irq_handler_t handler, unsigned long irqflags,
			 const char *devname, void *dev_id)
{
	struct irqaction *action;
	struct irq_desc *desc;
	int retval;

	// if (irq == IRQ_NOTCONNECTED)
	// 	return -ENOTCONN;

	/*
	 * Sanity-check: shared interrupts must pass in a real dev-ID,
	 * otherwise we'll have trouble later trying to figure out
	 * which interrupt is which (messes up the interrupt freeing
	 * logic etc).
	 *
	 * Also shared interrupts do not go well with disabling auto enable.
	 * The sharing interrupt might request it while it's still disabled
	 * and then wait for interrupts forever.
	 *
	 * Also IRQF_COND_SUSPEND only makes sense for shared interrupts and
	 * it cannot be set along with IRQF_NO_SUSPEND.
	 */
	// if (((irqflags & IRQF_SHARED) && !dev_id) ||
	//     ((irqflags & IRQF_SHARED) && (irqflags & IRQF_NO_AUTOEN)) ||
	//     (!(irqflags & IRQF_SHARED) && (irqflags & IRQF_COND_SUSPEND)) ||
	//     ((irqflags & IRQF_NO_SUSPEND) && (irqflags & IRQF_COND_SUSPEND)))
	// 	return -EINVAL;

	desc = irq_to_desc(irq);
	if (!desc)
		return -EINVAL;

	// if (!irq_settings_can_request(desc) ||
	//     WARN_ON(irq_settings_is_per_cpu_devid(desc)))
	// 	return -EINVAL;

	if (!handler) {
		// if (!thread_fn)
			// return -EINVAL;
		// handler = irq_default_primary_handler;
	}

	action = kzmalloc(sizeof(struct irqaction));
	if (!action)
		return -ENOMEM;

	action->handler = handler;
	// action->flags = irqflags;
	action->name = devname;
	action->dev_id = dev_id;

	// retval = irq_chip_pm_get(&desc->irq_data);
	// if (retval < 0) {
		// kfree(action);
		// return retval;
	// }

	retval = __setup_irq(irq, desc, action);

	if (retval) {
		// irq_chip_pm_put(&desc->irq_data);
		// kfree(action->secondary);
		kfree(action, sizeof(struct irqaction));
	}

	return retval;
}

void __disable_irq(struct irq_desc *desc)
{
	if (!desc->depth++)
		irq_disable(desc);
}

void __enable_irq(struct irq_desc *desc)
{
	switch (desc->depth) {
	case 0:
 err_out:
		printk("Unbalanced enable for IRQ %d\n", irq_desc_get_irq(desc));
		break;
	case 1: {
		// if (desc->istate & IRQS_SUSPENDED)
		// 	goto err_out;
		/* Prevent probing on this irq: */
		// irq_settings_set_noprobe(desc);
		/*
		 * Call irq_startup() not irq_enable() here because the
		 * interrupt might be marked NOAUTOEN. So irq_startup()
		 * needs to be invoked when it gets enabled the first
		 * time. If it was already started up, then irq_startup()
		 * will invoke irq_enable() under the hood.
		 */
		irq_startup(desc, IRQ_RESEND, IRQ_START_FORCE);
		break;
	}
	default:
		desc->depth--;
	}
}
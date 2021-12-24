#include <jnix/irq.h>
#include <jnix/interrupt.h>
#include <jnix/irqnr.h>
#include <mm/kmalloc.h>

static int __setup_irq(unsigned int irq, struct irq_desc *desc, struct irqaction *new)
{
	if (!desc)
		return -1;

	if (desc->irq_data.chip == &no_irq_chip)
		return -1;

	new->irq = irq;

	new->next = desc->action->next;
	desc->action->next = new;
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
		return -1;
		// return -EINVAL;

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
		return -1;
		// return -ENOMEM;

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
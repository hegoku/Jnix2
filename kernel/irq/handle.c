#include <jnix/irqdesc.h>
#include <jnix/printk.h>

void handle_bad_irq(struct irq_desc *desc)
{
	int irq = irq_desc_get_irq(desc);
	printk("irq:%#x\n", irq);
}

irqreturn_t handle_irq_event_percpu(struct irq_desc *desc)
{
	irqreturn_t retval = IRQ_NONE;
	unsigned int irq = desc->irq_data.irq;
	struct irqaction *action;

	// record_irq_time(desc);

	for_each_action_of_desc(desc, action) {
		irqreturn_t res;

		/*
		 * If this IRQ would be threaded under force_irqthreads, mark it so.
		 */
		// if (irq_settings_can_thread(desc) &&
		    // !(action->flags & (IRQF_NO_THREAD | IRQF_PERCPU | IRQF_ONESHOT)))
			// lockdep_hardirq_threaded();

		res = action->handler(irq, action->dev_id);

		// if (WARN_ONCE(!irqs_disabled(),"irq %u handler %pS enabled interrupts\n",
			    //   irq, action->handler))
			// local_irq_disable();

		// switch (res) {
		// case IRQ_WAKE_THREAD:
		// 	/*
		// 	 * Catch drivers which return WAKE_THREAD but
		// 	 * did not set up a thread function
		// 	 */
		// 	if (unlikely(!action->thread_fn)) {
		// 		warn_no_thread(irq, action);
		// 		break;
		// 	}

		// 	__irq_wake_thread(desc, action);

		// 	fallthrough;	/* to add to randomness */
		// case IRQ_HANDLED:
		// 	*flags |= action->flags;
		// 	break;

		// default:
		// 	break;
		// }

		retval |= res;
	}

	return retval;
}

irqreturn_t handle_irq_event(struct irq_desc *desc)
{
	irqreturn_t ret;

	// desc->istate &= ~IRQS_PENDING;
	// irqd_set(&desc->irq_data, IRQD_IRQ_INPROGRESS);
	// raw_spin_unlock(&desc->lock);

	ret = handle_irq_event_percpu(desc);

	// raw_spin_lock(&desc->lock);
	// irqd_clear(&desc->irq_data, IRQD_IRQ_INPROGRESS);
	return ret;
}
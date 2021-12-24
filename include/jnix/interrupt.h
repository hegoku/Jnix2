#ifndef JNIX_INTERRUPT_H
#define JNIX_INTERRUPT_H

#include <jnix/irqreturn.h>
#include <jnix/irqdesc.h>

typedef irqreturn_t (* irq_handler_t)(int, void *);

struct irqaction {
	irq_handler_t handler;
	const char *name;
	void *dev_id;
	struct irqaction *next;
	int irq;
};

#define for_each_action_of_desc(desc, act)			\
	for (act = desc->action; act; act = act->next)


irqreturn_t handle_irq_event(struct irq_desc *desc);

extern int early_irq_init(void);
// extern int arch_probe_nr_irqs(void);
// extern int arch_early_irq_init(void);

extern int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags, const char *name, void *dev);

#endif
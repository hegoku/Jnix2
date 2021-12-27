#ifndef JNIX_INTERRUPT_H
#define JNIX_INTERRUPT_H

#include <jnix/irqreturn.h>
#include <jnix/irqdesc.h>

#define IRQF_SHARED		0x00000080
#define IRQF_PROBE_SHARED	0x00000100
#define __IRQF_TIMER		0x00000200
#define IRQF_PERCPU		0x00000400
#define IRQF_NOBALANCING	0x00000800
#define IRQF_IRQPOLL		0x00001000
#define IRQF_ONESHOT		0x00002000
#define IRQF_NO_SUSPEND		0x00004000
#define IRQF_FORCE_RESUME	0x00008000
#define IRQF_NO_THREAD		0x00010000
#define IRQF_EARLY_RESUME	0x00020000
#define IRQF_COND_SUSPEND	0x00040000
#define IRQF_NO_AUTOEN		0x00080000
#define IRQF_NO_DEBUG		0x00100000

#define IRQF_TIMER		(__IRQF_TIMER | IRQF_NO_SUSPEND | IRQF_NO_THREAD)


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
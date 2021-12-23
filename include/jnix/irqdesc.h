#ifndef JNIX_IRQDESC_H
#define JNIX_IRQDESC_H

#include <jnix/irq.h>
#include <jnix/interrupt.h>

struct irq_desc {
	irq_flow_handler_t handle_irq;
	struct irqaction	*action;
	unsigned int		depth;
	struct irq_data		irq_data;
	const char		*name;
};

static inline unsigned int irq_desc_get_irq(struct irq_desc *desc)
{
	return desc->irq_data.irq;
}

static inline struct irq_data *irq_desc_get_irq_data(struct irq_desc *desc)
{
	return &desc->irq_data;
}

static inline struct irq_chip *irq_desc_get_chip(struct irq_desc *desc)
{
	return desc->irq_data.chip;
}

static inline void *irq_desc_get_chip_data(struct irq_desc *desc)
{
	return desc->irq_data.chip_data;
}

// static inline void *irq_desc_get_handler_data(struct irq_desc *desc)
// {
// 	return desc->irq_common_data.handler_data;
// }

// static inline struct msi_desc *irq_desc_get_msi_desc(struct irq_desc *desc)
// {
// 	return desc->irq_common_data.msi_desc;
// }

extern struct irq_desc *irq_desc[NR_IRQS];

extern void generic_handle_irq_desc(struct irq_desc *desc);

#endif
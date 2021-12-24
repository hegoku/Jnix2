#ifndef JNIX_IRQ_H
#define JNIX_IRQ_H

#include <jnix/irqreturn.h>
#include <asm/irq.h>

struct irq_data {
	unsigned int        irq;
	unsigned long       hwirq;
	unsigned int        state_use_accessors;
	struct irq_chip     *chip;
	void            *handler_data;
	void            *chip_data;
};

struct irq_chip {
	const char  *name;
	unsigned int    (*irq_startup)(struct irq_data *data);
	void        (*irq_shutdown)(struct irq_data *data);
	void        (*irq_enable)(struct irq_data *data);
	void        (*irq_disable)(struct irq_data *data);

	void        (*irq_ack)(struct irq_data *data);
	void        (*irq_mask)(struct irq_data *data);
	void        (*irq_mask_ack)(struct irq_data *data);
	void        (*irq_unmask)(struct irq_data *data);
	void        (*irq_eoi)(struct irq_data *data);
};

typedef void (*irq_flow_handler_t)(struct irq_desc *desc);
typedef void (*irq_preflow_handler_t)(struct irq_data *data);

extern void
irq_set_chip_and_handler_name(unsigned int irq, struct irq_chip *chip,
			      irq_flow_handler_t handle, const char *name);
static inline void irq_set_chip_and_handler(unsigned int irq, struct irq_chip *chip,
					    irq_flow_handler_t handle)
{
	irq_set_chip_and_handler_name(irq, chip, handle, NULL);
}

extern void handle_level_irq(struct irq_desc *desc);
extern void handle_fasteoi_irq(struct irq_desc *desc);
extern void handle_edge_irq(struct irq_desc *desc);
extern void handle_edge_eoi_irq(struct irq_desc *desc);
extern void handle_simple_irq(struct irq_desc *desc);
extern void handle_bad_irq(struct irq_desc *desc);

extern irqreturn_t no_action(int cpl, void *dev_id);

extern int irq_set_chip(unsigned int irq, struct irq_chip *chip);
extern void __irq_set_handler(unsigned int irq, irq_flow_handler_t handle, int is_chained, const char *name);

extern struct irq_chip no_irq_chip;
extern struct irq_chip dummy_irq_chip;

#endif
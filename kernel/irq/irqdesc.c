#include <mm/kmalloc.h>
#include <jnix/irqdesc.h>
#include <asm-generic/sections.h>
#include <jnix/interrupt.h>
#include <asm/irq.h>

struct irq_desc *irq_desc[NR_IRQS];

static struct irq_desc *alloc_desc(int irq)
{
	struct irq_desc *desc;

	desc = kzmalloc(sizeof(*desc));
	if (!desc)
		return NULL;

	desc->name = NULL;
	desc->depth = 1;
	desc->irq_data.irq = irq;
	desc->irq_data.chip = &no_irq_chip;
	desc->irq_data.chip_data = NULL;
	desc->handle_irq = handle_bad_irq;

	return desc;
}

int __init early_irq_init(void)
{
	int i, initcnt;
	struct irq_desc *desc;

	/* Let arch update nr_irqs and return the nr of preallocated irqs */
	// initcnt = arch_probe_nr_irqs();
	initcnt = NR_IRQS;

	for (i = 0; i < initcnt; i++) {
		desc = alloc_desc(i);
		irq_desc[i] = desc;
	}
	// return arch_early_irq_init();
	return 0;
}

struct irq_desc *irq_to_desc(unsigned int irq)
{
	return (irq < NR_IRQS) ? irq_desc[irq] : NULL;
}

void generic_handle_irq_desc(struct irq_desc *desc)
{
	desc->handle_irq(desc);
}
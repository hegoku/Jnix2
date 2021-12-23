#include <asm/irq.h>
#include <jnix/irq.h>
#include <jnix/init.h>
#include <asm/i8259.h>
#include <jnix/irqdesc.h>
#include <jnix/printk.h>
#include <asm/hw_irq.h>
#include <asm/irq_vectors.h>
#include <jnix/irqnr.h>
#include <asm/desc.h>

vector_irq_t vector_irq = {
	[0 ... NR_VECTORS - 1] = VECTOR_UNUSED,
};

void __init init_IRQ(void)
{
	int i;

	for (i = 0; i < nr_legacy_irqs(); i++)
		vector_irq[ISA_IRQ_VECTOR(i)] = irq_to_desc(i);

	native_init_IRQ();
}

void __init init_ISA_irqs(void)
{
	struct irq_chip *chip = legacy_pic->chip;
	int i;
	legacy_pic->init(0);

	for (i = 0; i < nr_legacy_irqs(); i++)
		irq_set_chip_and_handler_name(i, chip, handle_level_irq, "XT");

	for (i=0;i<nr_legacy_irqs();i++) {
		struct irq_desc *desc = vector_irq[ISA_IRQ_VECTOR(i)];
		printk("v:%d IRQ:%d HWIRQ:%d name:%s chip:%s\n", ISA_IRQ_VECTOR(i), desc->irq_data.irq, desc->irq_data.hwirq, desc->name, desc->irq_data.chip->name);
	}
}

void __init native_init_IRQ(void)
{
	/* Execute any quirks before the call gates are initialised: */
	init_ISA_irqs();

	idt_setup_apic_and_irq_gates();
	// lapic_assign_system_vectors();

	// if (!acpi_ioapic && !of_ioapic && nr_legacy_irqs()) {
	// 	/* IRQ2 is cascade interrupt to second interrupt controller */
	// 	if (request_irq(2, no_action, IRQF_NO_THREAD, "cascade", NULL))
	// 		pr_err("%s: request_irq() failed\n", "cascade");
	// }
}
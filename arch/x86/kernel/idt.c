#include <jnix/init.h>
#include <asm/segment.h>
#include <asm/irq_vectors.h>
#include <asm/desc.h>
#include <asm/desc_defs.h>
#include <asm/page.h>

#define IDT_TABLE_SIZE		(IDT_ENTRIES * sizeof(gate_desc))

/* Must be page-aligned because the real IDT is used in the cpu entry area */
static gate_desc idt_table[IDT_ENTRIES] __attribute__ ((section(".bss..page_aligned"))) __attribute__((aligned(PAGE_SIZE)));

static struct desc_ptr idt_descr __attribute__ ((section(".data..ro_after_init"))) = {
	.size		= IDT_TABLE_SIZE - 1,
	.address	= (unsigned long) idt_table,
};

static __init void
idt_setup_from_table(gate_desc *idt, const struct idt_data *t, int size, int sys)
{
	gate_desc desc;

	for (; size > 0; t++, size--) {
		idt_init_desc(&desc, t);
		write_idt_entry(idt, t->vector, &desc);
		// if (sys)
			// set_bit(t->vector, system_vectors);
	}
}

static __init void set_intr_gate(unsigned int n, const void *addr)
{
	struct idt_data data;

	init_idt_data(&data, n, addr);

	idt_setup_from_table(idt_table, &data, 1, 0);
}

/**
 * idt_setup_early_handler - Initializes the idt table with early handlers
 */
void __init idt_setup_early_handler(void)
{
	int i;

	for (i = 0; i < NUM_EXCEPTION_VECTORS; i++)
		set_intr_gate(i, early_idt_handler_array[i]);
	for ( ; i < NR_VECTORS; i++)
		set_intr_gate(i, early_ignore_irq);
	load_idt(&idt_descr);
}
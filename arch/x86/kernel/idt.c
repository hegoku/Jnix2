#include <jnix/init.h>
#include <asm/segment.h>
#include <asm/irq_vectors.h>
#include <asm/desc.h>
#include <asm/desc_defs.h>
#include <asm/page.h>
#include <asm/trapnr.h>
#include <asm/fault.h>
#include <asm/hw_irq.h>
#include <asm/desc.h>
#include <asm/proto.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

#define IDT_TABLE_SIZE		(IDT_ENTRIES * sizeof(gate_desc))

#define DPL0		0x0
#define DPL3		0x3

#define DEFAULT_STACK	0

#define G(_vector, _addr, _ist, _type, _dpl, _segment)	\
	{						\
		.vector		= _vector,		\
		.bits.ist	= _ist,			\
		.bits.type	= _type,		\
		.bits.dpl	= _dpl,			\
		.bits.p		= 1,			\
		.addr		= _addr,		\
		.segment	= _segment,		\
	}

/* Interrupt gate */
#define INTG(_vector, _addr)				\
	G(_vector, _addr, DEFAULT_STACK, GATE_INTERRUPT, DPL0, __KERNEL_CS)

/* System interrupt gate */
#define SYSG(_vector, _addr)				\
	G(_vector, _addr, DEFAULT_STACK, GATE_INTERRUPT, DPL3, __KERNEL_CS)

#define TSKG(_vector, _gdt)				\
	G(_vector, NULL, DEFAULT_STACK, GATE_TASK, DPL0, _gdt << 3)

static const __initconst struct idt_data def_idts[] = {
	INTG(X86_TRAP_DE,		asm_exc_de),
	INTG(X86_TRAP_DB,		asm_exc_db),
	INTG(X86_TRAP_NMI,		asm_exc_nmi),
	INTG(X86_TRAP_BP,		asm_exc_bp),
	INTG(X86_TRAP_OF,		asm_exc_of),

	INTG(X86_TRAP_TS,		asm_exc_ts),
	INTG(X86_TRAP_NP,		asm_exc_np),
	INTG(X86_TRAP_SS,		asm_exc_ss),
	INTG(X86_TRAP_GP,		asm_exc_gp),
	INTG(X86_TRAP_PF,		asm_exc_page_fault),

	SYSG(IA32_SYSCALL_VECTOR,	entry_INT80_32),
};

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

void __init idt_setup_traps(void)
{
	idt_setup_from_table(idt_table, def_idts, ARRAY_SIZE(def_idts), 1);
}

void __init idt_setup_apic_and_irq_gates(void)
{
	int i;
	void *entry;

	// idt_setup_from_table(idt_table, apic_idts, ARRAY_SIZE(apic_idts), 1);

	for (i=FIRST_EXTERNAL_VECTOR; i<FIRST_SYSTEM_VECTOR; i++) {
		if (i==0x80)
			continue;
		entry = irq_entries_start + 8 * (i - FIRST_EXTERNAL_VECTOR);
		set_intr_gate(i, entry);
	}
	// for_each_clear_bit_from(i, system_vectors, FIRST_SYSTEM_VECTOR) {
	// 	entry = irq_entries_start + 8 * (i - FIRST_EXTERNAL_VECTOR);
	// 	set_intr_gate(i, entry);
	// }

	load_idt(&idt_descr);
}
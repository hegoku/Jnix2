#ifndef ASM_DESC_H
#define ASM_DESC_H

#include <asm/page.h>
#include <asm/segment.h>
#include <asm/desc_defs.h>
#include <lib/string.h>

#define load_idt(dtr)				asm volatile("lidt %0"::"m" (*dtr))
#define load_ldt(ldt)				asm volatile("lldt %0"::"m" (ldt))

#define set_ldt					native_set_ldt

#define write_idt_entry(dt, entry, g)		native_write_idt_entry(dt, entry, g)

struct gdt_page {
	struct desc_struct gdt[GDT_ENTRIES];
} __attribute__((aligned(PAGE_SIZE)));

extern void idt_setup_early_handler(void);
extern unsigned long system_vectors[];
extern void idt_setup_traps(void);


static inline void idt_init_desc(gate_desc *gate, const struct idt_data *d)
{
	unsigned long addr = (unsigned long) d->addr;

	gate->offset_low	= (u16) addr;
	gate->segment		= (u16) d->segment;
	gate->bits		= d->bits;
	gate->offset_middle	= (u16) (addr >> 16);
}

static inline void init_idt_data(struct idt_data *data, unsigned int n,
				 const void *addr)
{
	// BUG_ON(n > 0xFF);
	memset(data, 2, sizeof(*data));
	data->vector	= n;
	data->addr	= addr;
	data->segment	= __KERNEL_CS;
	data->bits.type	= GATE_INTERRUPT;
	data->bits.p	= 1;
}

static inline void native_write_idt_entry(gate_desc *idt, int entry, const gate_desc *gate)
{
	memcpy(&idt[entry], gate, sizeof(*gate));
}

static inline void native_write_ldt_entry(struct desc_struct *ldt, int entry, const void *desc)
{
	memcpy(&ldt[entry], desc, 8);
}

#endif
#include <jnix/irqdesc.h>
#include <asm/i8259.h>
#include <jnix/printk.h>

unsigned int cached_irq_mask = 0xffff;
static int i8259A_auto_eoi;

static void mask_8259A_irq(unsigned int irq)
{
	unsigned int mask = 1 << irq;
	unsigned long flags;

	cached_irq_mask |= mask;
	if (irq & 8)
		outb(cached_slave_mask, PIC_SLAVE_IMR);
	else
		outb(cached_master_mask, PIC_MASTER_IMR);
}

static void disable_8259A_irq(struct irq_data *data)
{
	mask_8259A_irq(data->irq);
}

static void unmask_8259A_irq(unsigned int irq)
{
	unsigned int mask = ~(1 << irq);
	unsigned long flags;

	cached_irq_mask &= mask;
	if (irq & 8)
		outb(cached_slave_mask, PIC_SLAVE_IMR);
	else
		outb(cached_master_mask, PIC_MASTER_IMR);
}

static void enable_8259A_irq(struct irq_data *data)
{
	unmask_8259A_irq(data->irq);
}

static int i8259A_irq_pending(unsigned int irq)
{
	unsigned int mask = 1<<irq;
	unsigned long flags;
	int ret;

	if (irq < 8)
		ret = inb(PIC_MASTER_CMD) & mask;
	else
		ret = inb(PIC_SLAVE_CMD) & (mask >> 8);

	return ret;
}

static void make_8259A_irq(unsigned int irq)
{
	// disable_irq_nosync(irq);
	// io_apic_irqs &= ~(1<<irq);
	irq_set_chip_and_handler(irq, &i8259A_chip, handle_level_irq);
	// enable_irq(irq);
	// lapic_assign_legacy_vector(irq, true);
}

static inline int i8259A_irq_real(unsigned int irq)
{
	int value;
	int irqmask = 1<<irq;

	if (irq < 8) {
		outb(0x0B, PIC_MASTER_CMD);	/* ISR register */
		value = inb(PIC_MASTER_CMD) & irqmask;
		outb(0x0A, PIC_MASTER_CMD);	/* back to the IRR register */
		return value;
	}
	outb(0x0B, PIC_SLAVE_CMD);	/* ISR register */
	value = inb(PIC_SLAVE_CMD) & (irqmask >> 8);
	outb(0x0A, PIC_SLAVE_CMD);	/* back to the IRR register */
	return value;
}

static void mask_and_ack_8259A(struct irq_data *data)
{
	unsigned int irq = data->irq;
	unsigned int irqmask = 1 << irq;
	unsigned long flags;

	/*
	 * Lightweight spurious IRQ detection. We do not want
	 * to overdo spurious IRQ handling - it's usually a sign
	 * of hardware problems, so we only do the checks we can
	 * do without slowing down good hardware unnecessarily.
	 *
	 * Note that IRQ7 and IRQ15 (the two spurious IRQs
	 * usually resulting from the 8259A-1|2 PICs) occur
	 * even if the IRQ is masked in the 8259A. Thus we
	 * can check spurious 8259A IRQs without doing the
	 * quite slow i8259A_irq_real() call for every IRQ.
	 * This does not cover 100% of spurious interrupts,
	 * but should be enough to warn the user that there
	 * is something bad going on ...
	 */
	if (cached_irq_mask & irqmask)
		goto spurious_8259A_irq;
	cached_irq_mask |= irqmask;

handle_real_irq:
	if (irq & 8) {
		inb(PIC_SLAVE_IMR);	/* DUMMY - (do we need this?) */
		outb(cached_slave_mask, PIC_SLAVE_IMR);
		/* 'Specific EOI' to slave */
		outb(0x60+(irq&7), PIC_SLAVE_CMD);
		 /* 'Specific EOI' to master-IRQ2 */
		outb(0x60+PIC_CASCADE_IR, PIC_MASTER_CMD);
	} else {
		inb(PIC_MASTER_IMR);	/* DUMMY - (do we need this?) */
		outb(cached_master_mask, PIC_MASTER_IMR);
		outb(0x60+irq, PIC_MASTER_CMD);	/* 'Specific EOI to master */
	}
	return;

spurious_8259A_irq:
	/*
	 * this is the slow path - should happen rarely.
	 */
	if (i8259A_irq_real(irq))
		/*
		 * oops, the IRQ _is_ in service according to the
		 * 8259A - not spurious, go handle it.
		 */
		goto handle_real_irq;

	{
		static int spurious_irq_mask;
		/*
		 * At this point we can be sure the IRQ is spurious,
		 * lets ACK and report it. [once per IRQ]
		 */
		if (!(spurious_irq_mask & irqmask)) {
			printk("spurious 8259A interrupt: IRQ%d.\n", irq);
			spurious_irq_mask |= irqmask;
		}
		// atomic_inc(&irq_err_count);
		/*
		 * Theoretically we do not have to handle this IRQ,
		 * but in Linux this does not cause problems and is
		 * simpler for us.
		 */
		goto handle_real_irq;
	}
}

struct irq_chip i8259A_chip = {
	.name		= "XT-PIC",
	.irq_mask	= disable_8259A_irq,
	.irq_disable	= disable_8259A_irq,
	.irq_unmask	= enable_8259A_irq,
	.irq_mask_ack	= mask_and_ack_8259A,
};

static void mask_8259A(void)
{
	unsigned long flags;

	outb(0xff, PIC_MASTER_IMR);	/* mask all of 8259A-1 */
	outb(0xff, PIC_SLAVE_IMR);	/* mask all of 8259A-2 */
}

static void unmask_8259A(void)
{
	unsigned long flags;

	outb(cached_master_mask, PIC_MASTER_IMR); /* restore master IRQ mask */
	outb(cached_slave_mask, PIC_SLAVE_IMR);	  /* restore slave IRQ mask */
}

static int probe_8259A(void)
{
	unsigned long flags;
	unsigned char probe_val = ~(1 << PIC_CASCADE_IR);
	unsigned char new_val;

	outb(0xff, PIC_SLAVE_IMR);	/* mask all of 8259A-2 */
	outb(probe_val, PIC_MASTER_IMR);
	new_val = inb(PIC_MASTER_IMR);
	if (new_val != probe_val) {
		legacy_pic = &null_legacy_pic;
	}

	return nr_legacy_irqs();
}

static void init_8259A(int auto_eoi)
{
	unsigned long flags;

	i8259A_auto_eoi = auto_eoi;

	outb(0xff, PIC_MASTER_IMR);	/* mask all of 8259A-1 */

	/*
	 * outb_pic - this has to work on a wide range of PC hardware.
	 */
	outb_pic(0x11, PIC_MASTER_CMD);	/* ICW1: select 8259A-1 init */

	/* ICW2: 8259A-1 IR0-7 mapped to ISA_IRQ_VECTOR(0) */
	outb_pic(ISA_IRQ_VECTOR(0), PIC_MASTER_IMR);

	/* 8259A-1 (the master) has a slave on IR2 */
	outb_pic(1U << PIC_CASCADE_IR, PIC_MASTER_IMR);

	if (auto_eoi)	/* master does Auto EOI */
		outb_pic(MASTER_ICW4_DEFAULT | PIC_ICW4_AEOI, PIC_MASTER_IMR);
	else		/* master expects normal EOI */
		outb_pic(MASTER_ICW4_DEFAULT, PIC_MASTER_IMR);

	outb_pic(0x11, PIC_SLAVE_CMD);	/* ICW1: select 8259A-2 init */

	/* ICW2: 8259A-2 IR0-7 mapped to ISA_IRQ_VECTOR(8) */
	outb_pic(ISA_IRQ_VECTOR(8), PIC_SLAVE_IMR);
	/* 8259A-2 is a slave on master's IR2 */
	outb_pic(PIC_CASCADE_IR, PIC_SLAVE_IMR);
	/* (slave's support for AEOI in flat mode is to be investigated) */
	outb_pic(SLAVE_ICW4_DEFAULT, PIC_SLAVE_IMR);

	if (auto_eoi)
		/*
		 * In AEOI mode we just have to mask the interrupt
		 * when acking.
		 */
		i8259A_chip.irq_mask_ack = disable_8259A_irq;
	else
		i8259A_chip.irq_mask_ack = mask_and_ack_8259A;

	outb(cached_master_mask, PIC_MASTER_IMR); /* restore master IRQ mask */
	outb(cached_slave_mask, PIC_SLAVE_IMR);	  /* restore slave IRQ mask */

}

static void legacy_pic_noop(void) { };
static void legacy_pic_uint_noop(unsigned int unused) { };
static void legacy_pic_int_noop(int unused) { };
static int legacy_pic_irq_pending_noop(unsigned int irq)
{
	return 0;
}
static int legacy_pic_probe(void)
{
	return 0;
}
struct legacy_pic null_legacy_pic = {
	.nr_legacy_irqs = 0,
	.chip = &dummy_irq_chip,
	.mask = legacy_pic_uint_noop,
	.unmask = legacy_pic_uint_noop,
	.mask_all = legacy_pic_noop,
	.restore_mask = legacy_pic_noop,
	.init = legacy_pic_int_noop,
	.probe = legacy_pic_probe,
	.irq_pending = legacy_pic_irq_pending_noop,
	.make_irq = legacy_pic_uint_noop,
};

struct legacy_pic default_legacy_pic = {
	.nr_legacy_irqs = NR_IRQS_LEGACY,
	.chip  = &i8259A_chip,
	.mask = mask_8259A_irq,
	.unmask = unmask_8259A_irq,
	.mask_all = mask_8259A,
	.restore_mask = unmask_8259A,
	.init = init_8259A,
	.probe = probe_8259A,
	.irq_pending = i8259A_irq_pending,
	.make_irq = make_8259A_irq,
};

struct legacy_pic *legacy_pic = &default_legacy_pic;

// static int __init i8259A_init_ops(void)
// {
// 	init_8259A(0);
// 	return 0;
// }

// device_initcall(i8259A_init_ops);
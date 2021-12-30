#include <jnix/jiffies.h>
#include <jnix/init.h>
#include <jnix/interrupt.h>
#include <jnix/printk.h>
#include <jnix/i8253.h>
#include <jnix/sched.h>

u64 jiffies_64 = INITIAL_JIFFIES;

static irqreturn_t timer_interrupt(int irq, void *dev_id)
{
	// global_clock_event->event_handler(global_clock_event);
	// printk("#");
	schedule();
	return IRQ_HANDLED;
}

static void __init setup_default_timer_irq(void)
{
	unsigned long flags = IRQF_NOBALANCING | IRQF_IRQPOLL | IRQF_TIMER;

	/*
	 * Unconditionally register the legacy timer interrupt; even
	 * without legacy PIC/PIT we need this for the HPET0 in legacy
	 * replacement mode.
	 */
	if (request_irq(0, timer_interrupt, flags, "timer", NULL))
		printk("Failed to register legacy timer interrupt\n");
}

void __init hpet_time_init(void)
{
    // if (!hpet_enable())
        setup_pit_timer();
    setup_default_timer_irq();
}

static __init void x86_late_time_init(void)
{
	hpet_time_init();
}

void __init time_init(void)
{
    late_time_init = x86_late_time_init;
}
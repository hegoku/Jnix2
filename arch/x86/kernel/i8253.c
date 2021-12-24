#include <jnix/init.h>
#include <asm/io.h>
#include <jnix/i8253.h>

void __init setup_pit_timer(void)
{
    // clockevent_i8253_init(1); /*PIT芯片代号为8253*/
    // global_clock_event = &i8253_clockevent;
	outb(0x34, PIT_MODE);
    outb((unsigned char)(TIMER_FREQ / HZ), PIT_CH0);
    outb((unsigned char)((TIMER_FREQ / HZ)>>8), PIT_CH0);
}
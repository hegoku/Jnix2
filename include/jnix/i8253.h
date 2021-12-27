#ifndef __JNIX_I8253_H
#define __JNIX_I8253_H

#include <jnix/jiffies.h>

/* i8253A PIT registers */
#define PIT_MODE	0x43
#define PIT_CH0		0x40
#define PIT_CH2		0x42

#define PIT_TICK_RATE 1193182L
#define PIT_LATCH	((PIT_TICK_RATE + HZ/2) / HZ)

extern void setup_pit_timer(void);

#endif /* __LINUX_I8253_H */
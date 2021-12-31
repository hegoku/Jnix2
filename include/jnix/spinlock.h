#ifndef JNIX_SPINLOKC_H
#define JNIX_SPINLOKC_H

#include <jnix/irqflags.h>
#include <asm/spinlock.h>

typedef struct raw_spinlock {
    arch_spinlock_t raw_lock;
} raw_spinlock_t;

typedef struct spinlock {
    struct raw_spinlock rlock; 
} spinlock_t;

static inline void spin_lock_irq(spinlock_t *lock)
{
	local_irq_disable();
}

static inline void spin_unlock_irq(spinlock_t *lock)
{
    local_irq_enable();
}
#endif
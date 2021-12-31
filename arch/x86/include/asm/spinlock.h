#ifndef ARCH_X86_SPINLOCK_H
#define ARCH_X86_SPINLOCK_H

#include <jnix/types.h>

typedef struct arch_spinlock {
	u32 lock;
} arch_spinlock_t;

#endif
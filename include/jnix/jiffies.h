#ifndef JNIX_JIFFIES_H
#define JNIX_JIFFIES_H

#include <jnix/types.h>

#define HZ 100

#define INITIAL_JIFFIES ((unsigned long)(unsigned int) (-300*HZ))

extern u64 jiffies_64;
extern unsigned long volatile  jiffies;

#endif
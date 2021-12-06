#ifndef _JNIX_START_KERNEL_H
#define _JNIX_START_KERNEL_H

#include <jnix/init.h>

extern __attribute__((regparm(0))) void __init __attribute__((no_sanitize_address)) start_kernel(void);

#endif /* _JNIX_START_KERNEL_H */
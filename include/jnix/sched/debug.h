#ifndef _JNIX_SCHED_DEBUG_H
#define _JNIX_SCHED_DEBUG_H

/* Attach to any functions which should be ignored in wchan output. */
#define __sched		__section(".sched.text")

/* Linker adds these: start and end of __sched functions */
extern char __sched_text_start[], __sched_text_end[];

/* Is this address in the __sched functions? */
extern int in_sched_functions(unsigned long addr);

#endif
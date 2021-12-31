#ifndef _JNIX_SCHED_SIGNAL_H
#define _JNIX_SCHED_SIGNAL_H

#include <jnix/sched.h>

static inline int thread_group_leader(struct task_struct *p)
{
	return p->exit_signal >= 0;
}

#endif
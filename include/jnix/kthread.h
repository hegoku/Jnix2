#ifndef _JNIX_KTHREAD_H
#define _JNIX_KTHREAD_H

#include <jnix/sched.h>

int kthreadd(void *unused);
extern struct task_struct *kthreadd_task;

#endif
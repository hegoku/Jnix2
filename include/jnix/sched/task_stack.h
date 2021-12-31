#ifndef _JNIX_SCHED_TASK_STACK_H
#define _JNIX_SCHED_TASK_STACK_H

// #include <jnix/sched.h>

// static inline void *task_stack_page(const struct task_struct *task)
// {
// 	return task->stack;
// }

#define task_stack_page(task) (task->stack)

#endif
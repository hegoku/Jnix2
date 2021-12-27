#ifndef _JNIX_SCHED_TASK_H
#define _JNIX_SCHED_TASK_H

#include <jnix/sched.h>

#define CLONE_LEGACY_FLAGS 0xffffffffULL

struct kernel_clone_args {
	u64 flags;
	int *pidfd;
	int *child_tid;
	int *parent_tid;
	int exit_signal;
	unsigned long stack;
	unsigned long stack_size;
	unsigned long tls;
	pid_t *set_tid;
	/* Number of elements in *set_tid */
	size_t set_tid_size;
	int cgroup;
	int io_thread;
	// struct cgroup *cgrp;
	// struct css_set *cset;
};

extern union thread_union init_thread_union;
extern struct task_struct init_task;

extern pid_t kernel_clone(struct kernel_clone_args *kargs);
// struct task_struct *create_io_thread(int (*fn)(void *), void *arg, int node);
// struct task_struct *fork_idle(int);
// struct mm_struct *copy_init_mm(void);
extern pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags);
// extern long kernel_wait4(pid_t, int __user *, int, struct rusage *);
// int kernel_wait(pid_t pid, int *stat);

#endif /* _JNIX_SCHED_TASK_H */
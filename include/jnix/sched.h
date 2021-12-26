#ifndef _JNIX_SCHED_H
#define _JNIX_SCHED_H

#include <jnix/types.h>
#include <jnix/list.h>

#include <asm/page.h>
#include <asm/thread_info.h>

/* Used in tsk->state: */
#define TASK_RUNNING			0x0000
#define TASK_INTERRUPTIBLE		0x0001
#define TASK_UNINTERRUPTIBLE		0x0002
#define __TASK_STOPPED			0x0004
#define __TASK_TRACED			0x0008
/* Used in tsk->exit_state: */
#define EXIT_DEAD			0x0010
#define EXIT_ZOMBIE			0x0020
#define EXIT_TRACE			(EXIT_ZOMBIE | EXIT_DEAD)
/* Used in tsk->state again: */
#define TASK_PARKED			0x0040
#define TASK_DEAD			0x0080
#define TASK_WAKEKILL			0x0100
#define TASK_WAKING			0x0200
#define TASK_NOLOAD			0x0400
#define TASK_NEW			0x0800
/* RT specific auxilliary flag to mark RT lock waiters */
#define TASK_RTLOCK_WAIT		0x1000
#define TASK_STATE_MAX			0x2000

/* Convenience macros for the sake of set_current_state: */
#define TASK_KILLABLE			(TASK_WAKEKILL | TASK_UNINTERRUPTIBLE)
#define TASK_STOPPED			(TASK_WAKEKILL | __TASK_STOPPED)
#define TASK_TRACED			(TASK_WAKEKILL | __TASK_TRACED)

#define TASK_IDLE			(TASK_UNINTERRUPTIBLE | TASK_NOLOAD)

/* Convenience macros for the sake of wake_up(): */
#define TASK_NORMAL			(TASK_INTERRUPTIBLE | TASK_UNINTERRUPTIBLE)

/* get_task_state(): */
#define TASK_REPORT			(TASK_RUNNING | TASK_INTERRUPTIBLE | \
					 TASK_UNINTERRUPTIBLE | __TASK_STOPPED | \
					 __TASK_TRACED | EXIT_DEAD | EXIT_ZOMBIE | \
					 TASK_PARKED)

#define task_is_running(task)		((task->__state) == TASK_RUNNING)

#define task_is_traced(task)		(((ask->__state & __TASK_TRACED) != 0)

#define task_is_stopped(task)		((task->__state & __TASK_STOPPED) != 0)

#define task_is_stopped_or_traced(task)	((task->__state & (__TASK_STOPPED | __TASK_TRACED)) != 0)

struct task_struct {
	struct thread_info thread_info;
	unsigned int __state;
	void *stack;
	struct list_head tasks;

	// struct mm_struct *mm;

	int exit_state;
	int exit_code;
	int exit_signal;

	pid_t pid;
	pid_t tgid;

	struct task_struct *real_parent;

	/* Recipient of SIGCHLD, wait4() reports: */
	struct task_struct *parent;

	struct list_head children;
	struct list_head sibling;
	struct task_struct *group_leader;
};

union thread_union {
	struct task_struct task;
	struct thread_info thread_info;
	unsigned long stack[THREAD_SIZE/sizeof(long)];
};

extern struct thread_info init_thread_info;

extern unsigned long init_stack[THREAD_SIZE / sizeof(unsigned long)];

static inline struct thread_info *task_thread_info(struct task_struct *task)
{
	return &task->thread_info;
}

#endif
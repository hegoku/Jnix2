#ifndef _JNIX_SCHED_H
#define _JNIX_SCHED_H

#include <jnix/types.h>
#include <jnix/list.h>
#include <jnix/mm_types.h>

#include <asm/page.h>
#include <asm/thread_info.h>
#include <asm/processor.h>

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

#define get_current_state()	(current->__state)

#define TASK_COMM_LEN			16

/*
 * Per process flags
 */
#define PF_VCPU			0x00000001	/* I'm a virtual CPU */
#define PF_IDLE			0x00000002	/* I am an IDLE thread */
#define PF_EXITING		0x00000004	/* Getting shut down */
#define PF_IO_WORKER		0x00000010	/* Task is an IO worker */
#define PF_WQ_WORKER		0x00000020	/* I'm a workqueue worker */
#define PF_FORKNOEXEC		0x00000040	/* Forked but didn't exec */
#define PF_MCE_PROCESS		0x00000080      /* Process policy on mce errors */
#define PF_SUPERPRIV		0x00000100	/* Used super-user privileges */
#define PF_DUMPCORE		0x00000200	/* Dumped core */
#define PF_SIGNALED		0x00000400	/* Killed by a signal */
#define PF_MEMALLOC		0x00000800	/* Allocating memory */
#define PF_NPROC_EXCEEDED	0x00001000	/* set_user() noticed that RLIMIT_NPROC was exceeded */
#define PF_USED_MATH		0x00002000	/* If unset the fpu must be initialized before use */
#define PF_USED_ASYNC		0x00004000	/* Used async_schedule*(), used by module init */
#define PF_NOFREEZE		0x00008000	/* This thread should not be frozen */
#define PF_FROZEN		0x00010000	/* Frozen for system suspend */
#define PF_KSWAPD		0x00020000	/* I am kswapd */
#define PF_MEMALLOC_NOFS	0x00040000	/* All allocation requests will inherit GFP_NOFS */
#define PF_MEMALLOC_NOIO	0x00080000	/* All allocation requests will inherit GFP_NOIO */
#define PF_LOCAL_THROTTLE	0x00100000	/* Throttle writes only against the bdi I write to,
						 * I am cleaning dirty pages from some other bdi. */
#define PF_KTHREAD		0x00200000	/* I am a kernel thread */
#define PF_RANDOMIZE		0x00400000	/* Randomize virtual address space */
#define PF_SWAPWRITE		0x00800000	/* Allowed to write to swap */
#define PF_NO_SETAFFINITY	0x04000000	/* Userland is not allowed to meddle with cpus_mask */
#define PF_MCE_EARLY		0x08000000      /* Early kill for mce process policy */
#define PF_MEMALLOC_PIN		0x10000000	/* Allocation context constrained to zones which allow long term pinning. */
#define PF_FREEZER_SKIP		0x40000000	/* Freezer should not count it as freezable */
#define PF_SUSPEND_TASK		0x80000000      /* This thread called freeze_processes() and should not be frozen */

struct task_struct {
	struct thread_info thread_info;
	unsigned int __state;
	void *stack;
	struct list_head tasks;

	unsigned int flags;
	struct mm_struct *mm;
	struct mm_struct *active_mm;

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

	struct list_head rq;

	char comm[TASK_COMM_LEN];

	struct thread_struct thread;

};

union thread_union {
	unsigned long stack[THREAD_SIZE/sizeof(long)];
};

// extern struct thread_info init_thread_info;

# define task_thread_info(task)	(&(task)->thread_info)

extern unsigned long init_stack[THREAD_SIZE / sizeof(unsigned long)];

extern void __set_task_comm(struct task_struct *tsk, const char *from, int exec);

static inline void set_task_comm(struct task_struct *tsk, const char *from)
{
	__set_task_comm(tsk, from, 0);
}

extern char *__get_task_comm(char *to, size_t len, struct task_struct *tsk);
#define get_task_comm(buf, tsk) ({			\
	__get_task_comm(buf, sizeof(buf), tsk);		\
})

__attribute__((regparm(0))) void schedule(void);

extern void wake_up_new_task(struct task_struct *tsk);

#endif
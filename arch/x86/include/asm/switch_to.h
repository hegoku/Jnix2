#ifndef _ASM_X86_SWITCH_TO_H
#define _ASM_X86_SWITCH_TO_H

#include <jnix/sched.h>
#include <asm/ptrace.h>

__attribute__((regparm(0))) void ret_from_fork(void);

struct inactive_task_frame {
#ifdef CONFIG_X86_64
	unsigned long r15;
	unsigned long r14;
	unsigned long r13;
	unsigned long r12;
#else
	unsigned long flags;
	unsigned long si;
	unsigned long di;
#endif
	unsigned long bx;

	/*
	 * These two fields must be together.  They form a stack frame header,
	 * needed by get_frame_pointer().
	 */
	unsigned long bp;
	unsigned long ret_addr;
};

struct fork_frame {
	struct inactive_task_frame frame;
	struct pt_regs regs;
};

__attribute__((regparm(2))) struct task_struct *__switch_to_asm(struct task_struct *prev,
				    struct task_struct *next);

__attribute__((regparm(2))) struct task_struct *__switch_to(struct task_struct *prev,
					  struct task_struct *next);

#define switch_to(prev, next, last)					\
do {									\
	((last) = __switch_to_asm((prev), (next)));			\
} while (0)

#endif
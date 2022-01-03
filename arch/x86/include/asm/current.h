#ifndef _ASM_X86_CURRENT_H
#define _ASM_X86_CURRENT_H

#ifndef __ASSEMBLER__
#include <jnix/sched.h>

struct task_struct;

extern struct task_struct * current_task;

static inline struct task_struct *get_current(void)
{
	return current_task;
}

#define current get_current()

#endif /* __ASSEMBLY__ */

#endif /* _ASM_X86_CURRENT_H */
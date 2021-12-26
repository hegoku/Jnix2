#ifndef _ASM_X86_THREAD_INFO_H
#define _ASM_X86_THREAD_INFO_H

#include <jnix/types.h>

#ifdef CONFIG_X86_64
# define TOP_OF_KERNEL_STACK_PADDING 0
#else
#  define TOP_OF_KERNEL_STACK_PADDING 8
#endif

#ifndef __ASSEMBLER__

struct thread_info {
	unsigned long flags;		/* low level flags */
	unsigned long syscall_work;	/* SYSCALL_WORK_ flags */
	u32 status;		/* thread synchronous flags */
};

#define INIT_THREAD_INFO(tsk)			\
{						\
	.flags		= 0,			\
}

#else /* !__ASSEMBLY__ */

#endif /* __ASSEMBLER__ */
#endif
#ifndef _ASM_X86_THREAD_INFO_H
#define _ASM_X86_THREAD_INFO_H


#include <jnix/types.h>

#ifdef CONFIG_X86_64
# define TOP_OF_KERNEL_STACK_PADDING 0
#else
#  define TOP_OF_KERNEL_STACK_PADDING 8
#endif

#ifndef __ASSEMBLER__
#include <asm/switch_to.h>


struct thread_info {
	unsigned long flags;		/* low level flags */
	unsigned long syscall_work;	/* SYSCALL_WORK_ flags */
	u32 status;		/* thread synchronous flags */
};

#define INIT_THREAD_INFO(tsk)			\
{						\
	.flags		= 0,			\
}

static inline void kthread_frame_init(struct inactive_task_frame *frame,
				      unsigned long fun, unsigned long arg)
{
	frame->bx = fun;
#ifdef CONFIG_X86_64
	frame->r12 = arg;
#else
	frame->di = arg;
#endif
}

#else /* !__ASSEMBLY__ */

#endif /* __ASSEMBLER__ */
#endif
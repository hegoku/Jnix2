#include <asm/processor.h>
#include <asm/segment.h>
#include <lib/string.h>
#include <jnix/sched.h>
#include <jnix/sched/task.h>
#include <asm/ptrace.h>
#include <jnix/list.h>
#include <asm/switch_to.h>
#include <uapi/jnix/sched.h>
#include <asm/current.h>
#include <asm/segment.h>

#define BITS_PER_LONG 32

struct tss_struct cpu_tss_rw = {
	.x86_tss.sp0 = (1UL << (BITS_PER_LONG-1)) + 1,
	.x86_tss.sp1 = TOP_OF_INIT_STACK,

	.x86_tss.ss0 = __KERNEL_DS,
	.x86_tss.ss1 = __KERNEL_CS,
};

// int arch_dup_task_struct(struct task_struct *dst, struct task_struct *src)
// {
// 	memcpy(dst, src, sizeof(struct task_struct));

// 	return 0;
// }

static inline unsigned long encode_frame_pointer(struct pt_regs *regs)
{
	return (unsigned long)regs & 0x7fffffff;
}

#ifndef current_pt_regs
#define current_pt_regs() task_pt_regs(current)
#endif

int copy_thread(unsigned long clone_flags, unsigned long sp, unsigned long arg, struct task_struct *p, unsigned long tls)
{
	struct inactive_task_frame *frame;
	struct fork_frame *fork_frame;
	struct pt_regs *childregs;
	int ret = 0;

	childregs = task_pt_regs(p);
	fork_frame = container_of(childregs, struct fork_frame, regs);
	frame = &fork_frame->frame;

	frame->bp = encode_frame_pointer(childregs);
	frame->ret_addr = (unsigned long) ret_from_fork;
	p->thread.sp = (unsigned long) fork_frame;
	// p->thread.io_bitmap = NULL;
	// p->thread.iopl_warn = 0;
	// memset(p->thread.ptrace_bps, 0, sizeof(p->thread.ptrace_bps));

#ifdef CONFIG_X86_64
	// current_save_fsgs();
	p->thread.fsindex = current->thread.fsindex;
	p->thread.fsbase = current->thread.fsbase;
	p->thread.gsindex = current->thread.gsindex;
	p->thread.gsbase = current->thread.gsbase;

	// savesegment(es, p->thread.es);
	// savesegment(ds, p->thread.ds);
#else
	p->thread.sp0 = (unsigned long) (childregs + 1);
	/*
	 * Clear all status flags including IF and set fixed bit. 64bit
	 * does not have this initialization as the frame does not contain
	 * flags. The flags consistency (especially vs. AC) is there
	 * ensured via objtool, which lacks 32bit support.
	 */
	frame->flags = X86_EFLAGS_FIXED;
#endif

	/* Kernel thread ? */
	if (p->flags & PF_KTHREAD) {
		// p->thread.pkru = pkru_get_init_value();
		memset(childregs, 0, sizeof(struct pt_regs));
		kthread_frame_init(frame, sp, arg);
		return 0;
	}

	frame->bx = 0;
	*childregs = *current_pt_regs();
	childregs->ax = 0;

	if (sp)
		childregs->sp = sp;

#ifndef CONFIG_X86_64
	task_user_gs(p) = get_user_gs(current_pt_regs());
#endif

	if (p->flags & PF_IO_WORKER) {
		/*
		 * An IO thread is a user space thread, but it doesn't
		 * return to ret_after_fork().
		 *
		 * In order to indicate that to tools like gdb,
		 * we reset the stack and instruction pointers.
		 *
		 * It does the same kernel frame setup to return to a kernel
		 * function that a kernel thread does.
		 */
		childregs->sp = 0;
		childregs->ip = 0;
		kthread_frame_init(frame, sp, arg);
		return 0;
	}

	/* Set a new TLS for the child thread? */
	// if (clone_flags & CLONE_SETTLS)
		// ret = set_new_tls(p, tls);

	// if (!ret && unlikely(test_tsk_thread_flag(current, TIF_IO_BITMAP)))
	// 	io_bitmap_share(p);

	return ret;
}
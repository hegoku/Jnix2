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
#include <jnix/printk.h>

__attribute__((regparm(2))) struct task_struct *__switch_to(struct task_struct *prev_p, struct task_struct *next_p)
{
	struct thread_struct *prev = &prev_p->thread, *next = &next_p->thread;

	// printk("before: %x %x \n", &(next_p->thread.sp), next_p->thread.sp);
	// printk("before: %x %x %x\n", next_p, &(next_p->thread.sp0), next_p->thread.sp0);
	// printk("prev: %x %x %x\n", prev_p, &(prev_p->thread.sp0), prev_p->thread.sp0);

	cpu_tss_rw.x86_tss.sp1 = next_p->thread.sp0;

	cpu_current_top_of_stack = (unsigned long)task_stack_page(next_p) + THREAD_SIZE;
	current_task = next_p;
	return prev_p;
}
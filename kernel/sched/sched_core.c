#include <jnix/sched/init.h>
#include <jnix/init.h>
#include <asm/current.h>
#include <jnix/sched/debug.h>
#include <asm/irqflags.h>
#include <jnix/list.h>
#include <jnix/sched.h>
#include <jnix/irqflags.h>
#include <jnix/printk.h>
#include <asm/switch_to.h>

#define SM_NONE			0x0

struct rq {
	struct list_head *task;
	struct task_struct *curr;
};

struct rq rq = {
	.task = NULL,
	.curr = NULL
};

void __init sched_init(void)
{
	rq.curr = current;
	rq.task = &current->rq;
}

// static inline struct rq * context_switch(struct rq *rq, struct task_struct *prev, struct task_struct *next, struct rq_flags *rf)
static inline struct rq * context_switch(struct rq *rq, struct task_struct *prev, struct task_struct *next)
{
	// arch_start_context_switch(prev);
	// if (!next->mm) {
	// }
	// printk("before: %x %x \n", &(next->thread.sp), next->thread.sp);
	// printk("before: %x %x %x\n", next, &(next->thread.sp0), next->thread.sp0);
	// printk("prev: %x %x %x\n", prev, &(prev->thread.sp0), prev->thread.sp0);
	switch_to(prev, next, prev);
	// printk("after: %x %x %x\n", prev, next, current);
	return rq;
}

static void __sched notrace __schedule(unsigned int sched_mode)
{
	struct task_struct *prev, *next;

	prev = rq.curr;

	local_irq_disable();
	next = list_next_entry(prev, rq);
	if (prev!=next) {
		rq.curr = next;
		// printk("%d ", rq.curr->pid);
		context_switch(&rq, prev, next);
	}
}

__attribute__((regparm(0))) void __sched schedule(void)
{
	struct task_struct *tsk = current;

	// sched_submit_work(tsk);
	// do {
		// preempt_disable();
		__schedule(SM_NONE);
		// sched_preempt_enable_no_resched();
	// } while (need_resched());
	// sched_update_worker(tsk);
}

void wake_up_new_task(struct task_struct *p)
{
	p->__state = TASK_RUNNING;
	list_add_tail(&p->rq, rq.task);
}
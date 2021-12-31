#include <jnix/sched.h>
#include <jnix/kthread.h>
#include <asm/current.h>
#include <jnix/printk.h>

struct task_struct *kthreadd_task;

int kthreadd(void *unused)
{
	struct task_struct *tsk = current;

	set_task_comm(tsk, "kthreadd");

	current->flags |= PF_NOFREEZE;

	for(;;) {
		// printk("threadd");
		schedule();
	}

	return 0;
}
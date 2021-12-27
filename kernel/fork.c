#include <jnix/sched/task.h>
#include <jnix/kernel.h>
#include <uapi/jnix/sched.h>
#include <mm/kmalloc.h>
#include <mm/page.h>
#include <asm/current.h>
#include <jnix/sched/signal.h>
#include <jnix/printk.h>

unsigned int pid_count = 0;

int __attribute__((weak)) arch_dup_task_struct(struct task_struct *dst,
					       struct task_struct *src)
{
	*dst = *src;
	return 0;
}

static unsigned long *alloc_thread_stack_node(struct task_struct *tsk)
{
	struct page *page = alloc_pages(2);
	tsk->stack = __va((page-mem_map)<<PAGE_SHIFT);
	return tsk->stack;
}

static struct task_struct *dup_task_struct(struct task_struct *orig)
{
	struct task_struct *tsk;
	unsigned long *stack;

	tsk = kzmalloc(sizeof(struct task_struct));
	stack = alloc_thread_stack_node(tsk);

	arch_dup_task_struct(tsk, orig);

	tsk->stack = stack;
	return tsk;
}

// static struct task_struct *copy_process(struct pid *pid, struct kernel_clone_args *args)
static struct task_struct *copy_process(void *pid, struct kernel_clone_args *args)
{
	struct task_struct *p;
	u64 clone_flags = args->flags;

	p = dup_task_struct(current);

	// p->flags &= ~(PF_SUPERPRIV | PF_WQ_WORKER | PF_IDLE | PF_NO_SETAFFINITY);
	// p->flags |= PF_FORKNOEXEC;
	INIT_LIST_HEAD(&p->children);
	INIT_LIST_HEAD(&p->sibling);

	// p->utime = p->stime = p->gtime = 0;

	// p->pid = pid_nr(pid);
	p->pid = ++pid_count;
	if (clone_flags & CLONE_THREAD) {
		p->group_leader = current->group_leader;
		p->tgid = current->tgid;
	} else {
		p->group_leader = p;
		p->tgid = p->pid;
	}

	if (clone_flags & (CLONE_PARENT|CLONE_THREAD)) {
		p->real_parent = current->real_parent;
		// p->parent_exec_id = current->parent_exec_id;
		if (clone_flags & CLONE_THREAD)
			p->exit_signal = -1;
		else
			p->exit_signal = current->group_leader->exit_signal;
	} else {
		p->real_parent = current;
		// p->parent_exec_id = current->self_exec_id;
		p->exit_signal = args->exit_signal;
	}

	if (thread_group_leader(p)) {
		list_add_tail(&p->sibling, &p->real_parent->children);
		list_add_tail(&p->tasks, &init_task.tasks);
	} else {
		// list_add_tail_rcu(&p->thread_group,
					//   &p->group_leader->thread_group);
		// list_add_tail_rcu(&p->thread_node,
					// &p->signal->thread_head);
	}

	return p;
}

pid_t kernel_clone(struct kernel_clone_args *args)
{
	u64 clone_flags = args->flags;
	// struct completion vfork;
	struct pid *pid;
	struct task_struct *p;
	int trace = 0;
	pid_t nr;

	/*
	 * For legacy clone() calls, CLONE_PIDFD uses the parent_tid argument
	 * to return the pidfd. Hence, CLONE_PIDFD and CLONE_PARENT_SETTID are
	 * mutually exclusive. With clone3() CLONE_PIDFD has grown a separate
	 * field in struct clone_args and it still doesn't make sense to have
	 * them both point at the same memory location. Performing this check
	 * here has the advantage that we don't need to have a separate helper
	 * to check for legacy clone().
	 */
	if ((args->flags & CLONE_PIDFD) &&
	    (args->flags & CLONE_PARENT_SETTID) &&
	    (args->pidfd == args->parent_tid))
		// return -EINVAL;
		return -1;

	/*
	 * Determine whether and which event to report to ptracer.  When
	 * called from kernel_thread or CLONE_UNTRACED is explicitly
	 * requested, no event is reported; otherwise, report if the event
	 * for the type of forking is enabled.
	 */
	// if (!(clone_flags & CLONE_UNTRACED)) {
	// 	if (clone_flags & CLONE_VFORK)
	// 		trace = PTRACE_EVENT_VFORK;
	// 	else if (args->exit_signal != SIGCHLD)
	// 		trace = PTRACE_EVENT_CLONE;
	// 	else
	// 		trace = PTRACE_EVENT_FORK;

	// 	if (likely(!ptrace_event_enabled(current, trace)))
	// 		trace = 0;
	// }

	p = copy_process(NULL, args);
	// add_latent_entropy();

	if (p==NULL)
		return -1;

	/*
	 * Do this prior waking up the new thread - the thread pointer
	 * might get invalid after that point, if the thread exits quickly.
	 */
	// trace_sched_process_fork(current, p);

	// pid = get_task_pid(p, PIDTYPE_PID);
	// nr = pid_vnr(pid);
	nr = p->pid;

	// if (clone_flags & CLONE_PARENT_SETTID)
	// 	put_user(nr, args->parent_tid);

	// if (clone_flags & CLONE_VFORK) {
	// 	p->vfork_done = &vfork;
	// 	init_completion(&vfork);
	// 	get_task_struct(p);
	// }

	// wake_up_new_task(p);

	/* forking complete and child started to run, tell ptracer */
	// if (unlikely(trace))
	// 	ptrace_event_pid(trace, pid);

	// if (clone_flags & CLONE_VFORK) {
	// 	if (!wait_for_vfork_done(p, &vfork))
	// 		ptrace_event_pid(PTRACE_EVENT_VFORK_DONE, pid);
	// }

	// put_pid(pid);
	return nr;
}

pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags)
{
	struct kernel_clone_args args = {
		.flags		= ((lower_32_bits(flags) | CLONE_VM |
				    CLONE_UNTRACED) & ~CSIGNAL),
		.exit_signal	= (lower_32_bits(flags) & CSIGNAL),
		.stack		= (unsigned long)fn,
		.stack_size	= (unsigned long)arg,
	};

	return kernel_clone(&args);
}

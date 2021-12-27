#include <jnix/sched.h>
#include <mm/mm.h>
#include <jnix/list.h>
#include <jnix/sched/task.h>
#include <jnix/init_task.h>

struct task_struct init_task = {
	.thread_info = INIT_THREAD_INFO(init_task),
	.__state	= 0,
	.stack		= init_stack,
	// .mm		= NULL,
	// .active_mm	= &init_mm,
	.tasks		= LIST_HEAD_INIT(init_task.tasks),
	.real_parent	= &init_task,
	.parent		= &init_task,
	.children	= LIST_HEAD_INIT(init_task.children),
	.sibling	= LIST_HEAD_INIT(init_task.sibling),
	.group_leader	= &init_task,
	.comm		= INIT_TASK_COMM,
	.thread		= INIT_THREAD,
};

// struct thread_info init_thread_info __init_thread_info = INIT_THREAD_INFO(init_task);
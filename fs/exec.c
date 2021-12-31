#include <jnix/binfmts.h>
#include <jnix/sched.h>
#include <lib/string.h>

// static int bprm_execve(struct linux_binprm *bprm, int fd, struct filename *filename, int flags)
// {

// }

int kernel_execve(const char *kernel_filename, const char *const *argv, const char *const *envp)
{
	return 0;
}

char *__get_task_comm(char *buf, size_t buf_size, struct task_struct *tsk)
{
	// task_lock(tsk);
	strncpy(buf, tsk->comm, buf_size);
	// task_unlock(tsk);
	return buf;
}

/*
 * These functions flushes out all traces of the currently running executable
 * so that a new one can be started
 */

void __set_task_comm(struct task_struct *tsk, const char *buf, int exec)
{
	// task_lock(tsk);
	// trace_task_rename(tsk, buf);
	strlcpy(tsk->comm, buf, sizeof(tsk->comm));
	// task_unlock(tsk);
	// perf_event_comm(tsk, exec);
}
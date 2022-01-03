#include <asm/current.h>
#include <asm/syscall_wrapper.h>
#include <jnix/syscalls.h>
#include <jnix/printk.h>

SYSCALL_DEFINE0(getpid)
{
	return current->tgid;
}

SYSCALL_DEFINE0(gettid)
{
	return current->pid;
}

SYSCALL_DEFINE0(getppid)
{
	return current->real_parent->tgid;
}
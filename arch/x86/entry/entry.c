#include <asm/syscall.h>
#include <asm/ptrace.h>
#include <jnix/printk.h>
#include <jnix/syscalls.h>

extern long __i386_sys_getpid(const struct pt_regs *regs);
extern long __i386_sys_getppid(const struct pt_regs *regs);
extern long __i386_sys_gettid(const struct pt_regs *regs);

const sys_call_ptr_t sys_call_table[] = {
	[20] = __i386_sys_getpid,
	[64] = __i386_sys_getppid,
	[224] = __i386_sys_gettid
};

static inline int syscall_32_enter(struct pt_regs *regs)
{
	// if (IS_ENABLED(CONFIG_IA32_EMULATION))
		// current_thread_info()->status |= TS_COMPAT;

	return (int)regs->orig_ax;
}

static inline void do_syscall_32_irqs_on(struct pt_regs *regs, int nr)
{
	/*
	 * Convert negative numbers to very high and thus out of range
	 * numbers for comparisons.
	 */
	unsigned int unr = nr;

	// if (unr < IA32_NR_syscalls) { //449
	// unr = array_index_nospec(unr, IA32_NR_syscalls);
	regs->ax = sys_call_table[unr](regs);
	printk("syscall: %d\n", regs->ax);
	for (;;)
	{
	}
	// } else if (nr != -1) {
		// regs->ax = __ia32_sys_ni_syscall(regs);
	// }
}


__attribute__((regparm(1))) void do_int80_syscall_32(struct pt_regs *regs)
{
	int nr = syscall_32_enter(regs);

	// add_random_kstack_offset();
	/*
	 * Subtlety here: if ptrace pokes something larger than 2^31-1 into
	 * orig_ax, the int return value truncates it. This matches
	 * the semantics of syscall_get_nr().
	 */
	// nr = syscall_enter_from_user_mode(regs, nr);
	// instrumentation_begin();

	do_syscall_32_irqs_on(regs, nr);

	// instrumentation_end();
	// syscall_exit_to_user_mode(regs);
}
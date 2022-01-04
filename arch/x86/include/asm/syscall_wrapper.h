#ifndef _ASM_X86_SYSCALL_WRAPPER_H
#define _ASM_X86_SYSCALL_WRAPPER_H

#define __alias(symbol)                 __attribute__((__alias__(#symbol)))

#define SC_IA32_REGS_TO_ARGS(x, ...)					\
	__MAP(x,__SC_ARGS						\
	      ,,(unsigned int)regs->bx,,(unsigned int)regs->cx		\
	      ,,(unsigned int)regs->dx,,(unsigned int)regs->si		\
	      ,,(unsigned int)regs->di,,(unsigned int)regs->bp)

#define __SYS_STUB0(abi, name)                         \
	long __##abi##_##name(const struct pt_regs *regs); \
	long __##abi##_##name(const struct pt_regs *regs)  \
		__alias(__do_##name);

#define __SYS_STUBx(abi, name, ...)					\
	long __##abi##_##name(const struct pt_regs *regs);		\
	long __##abi##_##name(const struct pt_regs *regs)		\
	{								\
		return __se_##name(__VA_ARGS__);			\
	}

#define __I386_SYS_STUBx(x, name, ...)					\
	__SYS_STUBx(i386, sys##name,					\
		    SC_IA32_REGS_TO_ARGS(x, __VA_ARGS__))

#define __SYSCALL_DEFINEx(x, name, ...)					\
	static long __se_sys##name(__MAP(x,__SC_LONG,__VA_ARGS__));	\
	static inline long __do_sys##name(__MAP(x,__SC_DECL,__VA_ARGS__));\
	__I386_SYS_STUBx(x, name, __VA_ARGS__) \
	static long __se_sys##name(__MAP(x,__SC_LONG,__VA_ARGS__))	\
	{								\
		long ret = __do_sys##name(__MAP(x,__SC_CAST,__VA_ARGS__));\
		__MAP(x,__SC_TEST,__VA_ARGS__);				\
		return ret;						\
	}								\
	__attribute__((regparm(x))) long sys##name(__MAP(x,__SC_DECL,__VA_ARGS__))

/*
 * As the generic SYSCALL_DEFINE0() macro does not decode any parameters for
 * obvious reasons, and passing struct pt_regs *regs to it in %rdi does not
 * hurt, we only need to re-define it here to keep the naming congruent to
 * SYSCALL_DEFINEx() -- which is essential for the COND_SYSCALL() and SYS_NI()
 * macros to work correctly.
 */
#define SYSCALL_DEFINE0(sname) \
	static long __do_sys_##sname(const struct pt_regs *__unused); \
	__SYS_STUB0(i386, sys_##sname) \
	static long __do_sys_##sname(const struct pt_regs *__unused)

#endif
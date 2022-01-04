#ifndef _JNIX_SYSCALLS_H
#define _JNIX_SYSCALLS_H

#define __MAP0(m,...)
#define __MAP1(m,t,a,...) m(t,a)
#define __MAP2(m,t,a,...) m(t,a), __MAP1(m,__VA_ARGS__)
#define __MAP3(m,t,a,...) m(t,a), __MAP2(m,__VA_ARGS__)
#define __MAP4(m,t,a,...) m(t,a), __MAP3(m,__VA_ARGS__)
#define __MAP5(m,t,a,...) m(t,a), __MAP4(m,__VA_ARGS__)
#define __MAP6(m,t,a,...) m(t,a), __MAP5(m,__VA_ARGS__)
#define __MAP(n,...) __MAP##n(__VA_ARGS__)

#define __SC_DECL(t, a)    t a
#define __TYPE_AS(t, v)    __same_type((__force t)0, v)
#define __TYPE_IS_L(t)    (__TYPE_AS(t, 0L))
#define __TYPE_IS_UL(t)    (__TYPE_AS(t, 0UL))
#define __TYPE_IS_LL(t) (__TYPE_AS(t, 0LL) || __TYPE_AS(t, 0ULL))
#define __SC_LONG(t, a) __typeof(__builtin_choose_expr(__TYPE_IS_LL(t), 0LL, 0L)) a
#define __SC_CAST(t, a)    (__force t) a
#define __SC_ARGS(t, a)    a
#define __SC_TEST(t, a) (void)BUILD_BUG_ON_ZERO(!__TYPE_IS_LL(t) && sizeof(t) > sizeof(long))

#ifndef SYSCALL_DEFINE0
#define SYSCALL_DEFINE0(sname)					\
	__attribute__((regparm(0))) long sys_##sname(void);			\
	__attribute__((regparm(0))) long sys_##sname(void)
#endif /* SYSCALL_DEFINE0 */

#define SYSCALL_DEFINE1(name, ...) SYSCALL_DEFINEx(1, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE2(name, ...) SYSCALL_DEFINEx(2, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE3(name, ...) SYSCALL_DEFINEx(3, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE4(name, ...) SYSCALL_DEFINEx(4, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE5(name, ...) SYSCALL_DEFINEx(5, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE6(name, ...) SYSCALL_DEFINEx(6, _##name, __VA_ARGS__)

#define SYSCALL_DEFINE_MAXARGS	6

#define SYSCALL_DEFINEx(x, sname, ...)				\
	__SYSCALL_DEFINEx(x, sname, __VA_ARGS__)

/* kernel/sys.c */
__attribute__((regparm(0))) long sys_getpid(void);
__attribute__((regparm(0))) long sys_getppid(void);
// asmlinkage long sys_getuid(void);
// asmlinkage long sys_geteuid(void);
// asmlinkage long sys_getgid(void);
// asmlinkage long sys_getegid(void);
__attribute__((regparm(0))) long sys_gettid(void);
// asmlinkage long sys_sysinfo(struct sysinfo __user *info);

#endif
#ifndef _ASM_X86_SYSCALL_H
#define _ASM_X86_SYSCALL_H

#include <asm/ptrace.h>

typedef long (*sys_call_ptr_t)(const struct pt_regs *);
extern const sys_call_ptr_t sys_call_table[];

void do_syscall_64(struct pt_regs *regs, int nr);
__attribute__((regparm(1))) void do_int80_syscall_32(struct pt_regs *regs);
long do_fast_syscall_32(struct pt_regs *regs);

#endif
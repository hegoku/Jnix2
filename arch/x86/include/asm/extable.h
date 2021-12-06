#ifndef _ASM_X86_EXTABLE_H
#define _ASM_X86_EXTABLE_H

#include <jnix/init.h>
#include <asm/ptrace.h>


extern void early_fixup_exception(struct pt_regs *regs, int trapnr) __attribute__((regparm(3)));

#endif
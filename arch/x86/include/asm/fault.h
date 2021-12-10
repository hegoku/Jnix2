#ifndef ARCH_X86_ASM_FAULT_H
#define ARCH_X86_ASM_FAULT_H

#include <asm/ptrace.h>

extern void asm_exc_page_fault(void);
__attribute__((regparm(3))) void exc_page_fault(struct pt_regs *regs, unsigned long error_code);

#endif
#ifndef ARCH_X86_ASM_FAULT_H
#define ARCH_X86_ASM_FAULT_H

#include <asm/ptrace.h>

extern void asm_exc_de(void);
extern void asm_exc_db(void);
extern void asm_exc_nmi(void);
extern void asm_exc_bp(void);
extern void asm_exc_of(void);

extern void asm_exc_ts(void);
extern void asm_exc_np(void);
extern void asm_exc_ss(void);
extern void asm_exc_gp(void);
extern void asm_exc_page_fault(void);

__attribute__((regparm(3))) void exc_de(struct pt_regs *regs, unsigned long error_code);
__attribute__((regparm(3))) void exc_db(struct pt_regs *regs, unsigned long error_code);
__attribute__((regparm(3))) void exc_nmi(struct pt_regs *regs, unsigned long error_code);
__attribute__((regparm(3))) void exc_bp(struct pt_regs *regs, unsigned long error_code);
__attribute__((regparm(3))) void exc_of(struct pt_regs *regs, unsigned long error_code);

__attribute__((regparm(3))) void exc_ts(struct pt_regs *regs, unsigned long error_code);
__attribute__((regparm(3))) void exc_np(struct pt_regs *regs, unsigned long error_code);
__attribute__((regparm(3))) void exc_ss(struct pt_regs *regs, unsigned long error_code);
__attribute__((regparm(3))) void exc_gp(struct pt_regs *regs, unsigned long error_code);
__attribute__((regparm(3))) void exc_page_fault(struct pt_regs *regs, unsigned long error_code);

#endif
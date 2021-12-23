#ifndef ARCH_X86_IRQ_H
#define ARCH_X86_IRQ_H

#include <asm/irq_vectors.h>
#include <jnix/init.h>
#include <asm/ptrace.h>

struct irq_desc;

extern void init_ISA_irqs(void);
extern void native_init_IRQ(void);

extern void __handle_irq(struct irq_desc *desc, struct pt_regs *regs);

extern void __init init_IRQ(void);

#endif
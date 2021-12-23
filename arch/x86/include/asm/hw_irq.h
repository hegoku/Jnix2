#ifndef ARCH_X86_ASM_HW_IRQ_H
#define ARCH_X86_ASM_HW_IRQ_H
#include <asm/irq_vectors.h>

#define VECTOR_UNUSED		NULL
#define VECTOR_SHUTDOWN		((void *)-1L)
#define VECTOR_RETRIGGERED	((void *)-2L)

extern char irq_entries_start[];

typedef struct irq_desc* vector_irq_t[NR_VECTORS];
extern vector_irq_t vector_irq;
__attribute__((regparm(3))) void common_interrupt(struct pt_regs *regs, u32 vector);

#endif
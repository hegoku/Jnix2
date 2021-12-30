#ifndef _X86_IRQFLAGS_H_
#define _X86_IRQFLAGS_H_

#ifndef __ASSEMBLER__

#define __cpuidle __section(".cpuidle.text")

static inline void native_irq_disable(void)
{
	asm volatile("cli": : :"memory");
}

static inline void native_irq_enable(void)
{
	asm volatile("sti": : :"memory");
}

static inline __cpuidle void native_safe_halt(void)
{
	// mds_idle_clear_cpu_buffers();
	asm volatile("sti; hlt": : :"memory");
}

static inline __cpuidle void native_halt(void)
{
	// mds_idle_clear_cpu_buffers();
	asm volatile("hlt": : :"memory");
}

static inline void arch_local_irq_disable(void)
{
	native_irq_disable();
}

static inline void arch_local_irq_enable(void)
{
	native_irq_enable();
}

/*
 * Used in the idle loop; sti takes one instruction cycle
 * to complete:
 */
static inline __cpuidle void arch_safe_halt(void)
{
	native_safe_halt();
}

/*
 * Used when interrupts are already enabled or to
 * shutdown the processor:
 */
static inline __cpuidle void halt(void)
{
	native_halt();
}

#endif /*__ASSEMBLER__*/
#endif
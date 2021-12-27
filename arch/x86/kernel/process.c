#include <asm/processor.h>
#include <asm/segment.h>
#include <lib/string.h>
#include <jnix/sched.h>

#define BITS_PER_LONG 32

struct tss_struct cpu_tss_rw = {
	.x86_tss.sp0 = (1UL << (BITS_PER_LONG-1)) + 1,
	.x86_tss.sp1 = TOP_OF_INIT_STACK,

	.x86_tss.ss0 = __KERNEL_DS,
	.x86_tss.ss1 = __KERNEL_CS,
};

int arch_dup_task_struct(struct task_struct *dst, struct task_struct *src)
{
	memcpy(dst, src, sizeof(struct task_struct));

	return 0;
}
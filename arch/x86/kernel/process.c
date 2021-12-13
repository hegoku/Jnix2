#include <asm/processor.h>
#include <asm/segment.h>

#define BITS_PER_LONG 32

struct tss_struct cpu_tss_rw = {
	.sp0 = (1UL << (BITS_PER_LONG-1)) + 1,
	.sp1 = TOP_OF_INIT_STACK,

	.ss0 = __KERNEL_DS,
	.ss1 = __KERNEL_CS,
}
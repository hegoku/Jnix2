/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_SETUP_H
#define _ASM_X86_SETUP_H

#define COMMAND_LINE_SIZE 2048

#define PARAM_SIZE 4096		/* sizeof(struct boot_params) */

#define OLD_CL_MAGIC		0xA33F
#define OLD_CL_ADDRESS		0x020	/* Relative to real mode data */
#define NEW_CL_POINTER		0x228	/* Relative to real mode data */

#ifndef __ASSEMBLER__
#include <uapi/asm/bootparam.h>
#include <jnix/init.h>
#include <asm/desc.h>

#ifndef _SETUP
/*
 * This is set up by the setup-routine at boot-time
 */
extern struct boot_params boot_params;
extern char _text[];

extern struct __attribute__((section(".data..page_aligned"))) gdt_page gdt_page;

__attribute__((regparm(0))) void __init i386_start_kernel(void);

#define LOWMEMSIZE()	(0x9f000)

extern unsigned long _brk_end;
void *extend_brk(size_t size, size_t align);

#define RESERVE_BRK(name,sz)						\
	static void __section(".discard.text") __used notrace		\
	__brk_reservation_fn_##name##__(void) {				\
		asm volatile (						\
			".pushsection .brk_reservation,\"aw\",@nobits;" \
			".brk." #name ":"				\
			" 1:.skip %c0;"					\
			" .size .brk." #name ", . - 1b;"		\
			" .popsection"					\
			: : "i" (sz));					\
	}
	
#endif /* _SETUP */
#else
#define RESERVE_BRK(name,sz)				\
	.pushsection .brk_reservation,"aw",@nobits;	\
.brk.name:						\
1:	.skip sz;					\
	.size .brk.name,.-1b;				\
	.popsection
#endif /* __ASSEMBLY__ */
#endif /* _ASM_X86_SETUP_H */
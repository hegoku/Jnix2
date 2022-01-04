/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_SEGMENT_H
#define _ASM_X86_SEGMENT_H

/*
 * Constructor for a conventional segment GDT (or LDT) entry.
 * This is a macro so it can be used in initializers.
 */
#define GDT_ENTRY(flags, base, limit)			\
	((((base)  & 0xff000000ULL) << (56-24)) |	\
	 (((flags) & 0x0000f0ffULL) << 40) |	\
	 (((limit) & 0x000f0000ULL) << (48-16)) |	\
	 (((base)  & 0x00ffffffULL) << 16) |	\
	 (((limit) & 0x0000ffffULL)))

/* Simple and small GDT entries for booting only: */

#define GDT_ENTRY_BOOT_CS	2
#define GDT_ENTRY_BOOT_DS	3
#define GDT_ENTRY_BOOT_TSS	4
#define __BOOT_CS		(GDT_ENTRY_BOOT_CS*8)
#define __BOOT_DS		(GDT_ENTRY_BOOT_DS*8)
#define __BOOT_TSS		(GDT_ENTRY_BOOT_TSS*8)

/*
 * Bottom two bits of selector give the ring
 * privilege level
 */
#define SEGMENT_RPL_MASK	0x3

/*
 * When running on Xen PV, the actual privilege level of the kernel is 1,
 * not 0. Testing the Requested Privilege Level in a segment selector to
 * determine whether the context is user mode or kernel mode with
 * SEGMENT_RPL_MASK is wrong because the PV kernel's privilege level
 * matches the 0x3 mask.
 *
 * Testing with USER_SEGMENT_RPL_MASK is valid for both native and Xen PV
 * kernels because privilege level 2 is never used.
 */
#define USER_SEGMENT_RPL_MASK	0x2

/* User mode is privilege level 3: */
#define USER_RPL		0x3

/* Bit 2 is Table Indicator (TI): selects between LDT or GDT */
#define SEGMENT_TI_MASK		0x4
/* LDT segment has TI set ... */
#define SEGMENT_LDT		0x4
/* ... GDT has it cleared */
#define SEGMENT_GDT		0x0

#define GDT_ENTRY_INVALID_SEG	0

/*
 * The layout of the per-CPU GDT under Linux:
 *
 *   0 - null								<=== cacheline #1
 *   1 - reserved
 *   2 - reserved
 *   3 - reserved
 *
 *   4 - unused								<=== cacheline #2
 *   5 - unused
 *
 *  ------- start of TLS (Thread-Local Storage) segments:
 *
 *   6 - TLS segment #1			[ glibc's TLS segment ]
 *   7 - TLS segment #2			[ Wine's %fs Win32 segment ]
 *   8 - TLS segment #3							<=== cacheline #3
 *   9 - reserved
 *  10 - reserved
 *  11 - reserved
 *
 *  ------- start of kernel segments:
 *
 *  12 - kernel code segment						<=== cacheline #4
 *  13 - kernel data segment
 *  14 - default user CS
 *  15 - default user DS
 *  16 - TSS								<=== cacheline #5
 *  17 - LDT
 *  18 - PNPBIOS support (16->32 gate)
 *  19 - PNPBIOS support
 *  20 - PNPBIOS support						<=== cacheline #6
 *  21 - PNPBIOS support
 *  22 - PNPBIOS support
 *  23 - APM BIOS support
 *  24 - APM BIOS support						<=== cacheline #7
 *  25 - APM BIOS support
 *
 *  26 - ESPFIX small SS
 *  27 - per-cpu			[ offset to per-cpu data area ]
 *  28 - unused
 *  29 - unused
 *  30 - unused
 *  31 - TSS for double fault handler
 */
#define GDT_ENTRY_TLS_MIN		6
#define GDT_ENTRY_TLS_MAX 		(GDT_ENTRY_TLS_MIN + GDT_ENTRY_TLS_ENTRIES - 1)

#define GDT_ENTRY_KERNEL_CS		12
#define GDT_ENTRY_KERNEL_DS		13
#define GDT_ENTRY_DEFAULT_USER_CS	14
#define GDT_ENTRY_DEFAULT_USER_DS	15
#define GDT_ENTRY_TSS			16
#define GDT_ENTRY_LDT			17
#define GDT_ENTRY_PNPBIOS_CS32		18
#define GDT_ENTRY_PNPBIOS_CS16		19
#define GDT_ENTRY_PNPBIOS_DS		20
#define GDT_ENTRY_PNPBIOS_TS1		21
#define GDT_ENTRY_PNPBIOS_TS2		22
#define GDT_ENTRY_APMBIOS_BASE		23

#define GDT_ENTRY_ESPFIX_SS		26
#define GDT_ENTRY_PERCPU		27
#define GDT_ENTRY_STACK_CANARY		28

#define GDT_ENTRY_DOUBLEFAULT_TSS	31

/*
 * Number of entries in the GDT table:
 */
#define GDT_ENTRIES			32

/*
 * Segment selector values corresponding to the above entries:
 */

#define __KERNEL_CS			(GDT_ENTRY_KERNEL_CS*8)
#define __KERNEL_DS			(GDT_ENTRY_KERNEL_DS*8)
#define __USER_DS			(GDT_ENTRY_DEFAULT_USER_DS*8 + 3)
#define __USER_CS			(GDT_ENTRY_DEFAULT_USER_CS*8 + 3)
#define __ESPFIX_SS			(GDT_ENTRY_ESPFIX_SS*8)
#define __KERNEL_TSS		(GDT_ENTRY_TSS*8)

/* segment for calling fn: */
#define PNP_CS32			(GDT_ENTRY_PNPBIOS_CS32*8)
/* code segment for BIOS: */
#define PNP_CS16			(GDT_ENTRY_PNPBIOS_CS16*8)

/* "Is this PNP code selector (PNP_CS32 or PNP_CS16)?" */
#define SEGMENT_IS_PNP_CODE(x)		(((x) & 0xf4) == PNP_CS32)

/* data segment for BIOS: */
#define PNP_DS				(GDT_ENTRY_PNPBIOS_DS*8)
/* transfer data segment: */
#define PNP_TS1				(GDT_ENTRY_PNPBIOS_TS1*8)
/* another data segment: */
#define PNP_TS2				(GDT_ENTRY_PNPBIOS_TS2*8)

#ifdef CONFIG_SMP
# define __KERNEL_PERCPU		(GDT_ENTRY_PERCPU*8)
#else
# define __KERNEL_PERCPU		0
#endif

#ifdef CONFIG_STACKPROTECTOR
# define __KERNEL_STACK_CANARY		(GDT_ENTRY_STACK_CANARY*8)
#else
# define __KERNEL_STACK_CANARY		0
#endif


#define IDT_ENTRIES			256
#define NUM_EXCEPTION_VECTORS		32

/* Bitmask of exception vectors which push an error code on the stack: */
#define EXCEPTION_ERRCODE_MASK		0x20027d00

#define GDT_SIZE			(GDT_ENTRIES*8)
#define GDT_ENTRY_TLS_ENTRIES		3
#define TLS_SIZE			(GDT_ENTRY_TLS_ENTRIES* 8)

// #ifdef __KERNEL__

/*
 * early_idt_handler_array is an array of entry points referenced in the
 * early IDT.  For simplicity, it's a real array with one entry point
 * every nine bytes.  That leaves room for an optional 'push $0' if the
 * vector has no error code (two bytes), a 'push $vector_number' (two
 * bytes), and a jump to the common entry code (up to five bytes).
 */
#define EARLY_IDT_HANDLER_SIZE 9

/*
 * xen_early_idt_handler_array is for Xen pv guests: for each entry in
 * early_idt_handler_array it contains a prequel in the form of
 * pop %rcx; pop %r11; jmp early_idt_handler_array[i]; summing up to
 * max 8 bytes.
 */
#define XEN_EARLY_IDT_HANDLER_SIZE 8

#ifndef __ASSEMBLER__

extern const char early_idt_handler_array[NUM_EXCEPTION_VECTORS][EARLY_IDT_HANDLER_SIZE];
extern void early_ignore_irq(void);

/*
 * Load a segment. Fall back on loading the zero segment if something goes
 * wrong.  This variant assumes that loading zero fully clears the segment.
 * This is always the case on Intel CPUs and, even on 64-bit AMD CPUs, any
 * failure to fully clear the cached descriptor is only observable for
 * FS and GS.
 */
#define __loadsegment_simple(seg, value)				\
do {									\
	unsigned short __val = (value);					\
									\
	asm volatile("						\n"	\
		     "1:	movl %k0,%%" #seg "		\n"	\
									\
		     ".section .fixup,\"ax\"			\n"	\
		     "2:	xorl %k0,%k0			\n"	\
		     "		jmp 1b				\n"	\
		     ".previous					\n"	\
									\
		     _ASM_EXTABLE(1b, 2b)				\
									\
		     : "+r" (__val) : : "memory");			\
} while (0)

#define __loadsegment_ss(value) __loadsegment_simple(ss, (value))
#define __loadsegment_ds(value) __loadsegment_simple(ds, (value))
#define __loadsegment_es(value) __loadsegment_simple(es, (value))

#ifdef CONFIG_X86_64

// static inline void __loadsegment_fs(unsigned short value)
// {
// 	asm volatile("						\n"
// 		     "1:	movw %0, %%fs			\n"
// 		     "2:					\n"

// 		     _ASM_EXTABLE_TYPE(1b, 2b, EX_TYPE_CLEAR_FS)

// 		     : : "rm" (value) : "memory");
// }

/* __loadsegment_gs is intentionally undefined.  Use load_gs_index instead. */

#else

/*
 * On 32-bit systems, the hidden parts of FS and GS are unobservable if
 * the selector is NULL, so there's no funny business here.
 */
#define __loadsegment_fs(value) __loadsegment_simple(fs, (value))
#define __loadsegment_gs(value) __loadsegment_simple(gs, (value))

#endif

#define loadsegment(seg, value) __loadsegment_ ## seg (value)

/*
 * Save a segment register away:
 */
#define savesegment(seg, value)				\
	asm("mov %%" #seg ",%0":"=r" (value) : : "memory")

// #ifdef CONFIG_X86_32
# define get_user_gs(regs)		(u16)({ unsigned long v; savesegment(gs, v); v; })
# define set_user_gs(regs, v)		loadsegment(gs, (unsigned long)(v))
# define task_user_gs(tsk)		((tsk)->thread.gs)
# define lazy_save_gs(v)		savesegment(gs, (v))
# define lazy_load_gs(v)		loadsegment(gs, (v))
# define load_gs_index(v)		loadsegment(gs, (v))
// #endif	/* X86_32 */

#endif /* !__ASSEMBLY__ */
// #endif /* __KERNEL__ */

#endif /* _ASM_X86_SEGMENT_H */
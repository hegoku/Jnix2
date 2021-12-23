#ifndef _ASM_X86_PTRACE_H
#define _ASM_X86_PTRACE_H

#include <uapi/asm/processor-flags.h>
#include <asm/segment.h>

#define PT_BX 0
#define PT_CX 4
#define PT_DX 8
#define PT_SI 12
#define PT_DI 16
#define PT_DP 20
#define PT_AX 24
#define PT_DS 28
#define PT_DSH 30
#define PT_ES 32
#define PT_ESH 34
#define PT_FS 36
#define PT_FSH 36
#define PT_GS 40
#define PT_GSH 42
#define PT_ORIG_EAX 44
#define PT_IP 48
#define PT_CS 52
#define PT_CSH 54
#define PT_FLAGS 56
#define PT_SP 60
#define PT_SS 64
#define PT_SSH 66

#ifndef __ASSEMBLER__

struct pt_regs {
	/*
	 * NB: 32-bit x86 CPUs are inconsistent as what happens in the
	 * following cases (where %seg represents a segment register):
	 *
	 * - pushl %seg: some do a 16-bit write and leave the high
	 *   bits alone
	 * - movl %seg, [mem]: some do a 16-bit write despite the movl
	 * - IDT entry: some (e.g. 486) will leave the high bits of CS
	 *   and (if applicable) SS undefined.
	 *
	 * Fortunately, x86-32 doesn't read the high bits on POP or IRET,
	 * so we can just treat all of the segment registers as 16-bit
	 * values.
	 */
	unsigned long bx; //0x0
	unsigned long cx; //0x4
	unsigned long dx; //0x8
	unsigned long si; //12
	unsigned long di; //16
	unsigned long bp; //20
	unsigned long ax; //24
	unsigned short ds; //28
	unsigned short __dsh; //30
	unsigned short es; //32
	unsigned short __esh; //34
	unsigned short fs; //36
	unsigned short __fsh; //38
	/*
	 * On interrupt, gs and __gsh store the vector number.  They never
	 * store gs any more.
	 */
	unsigned short gs; //40
	unsigned short __gsh; //42
	/* On interrupt, this is the error code. */
	unsigned long orig_ax; //44
	unsigned long ip; //48
	unsigned short cs; //52
	unsigned short __csh; //54
	unsigned long flags; //56
	unsigned long sp; //60
	unsigned short ss; //64
	unsigned short __ssh; //66
};

/*
 * user_mode(regs) determines whether a register set came from user
 * mode.  On x86_32, this is true if V8086 mode was enabled OR if the
 * register set was from protected mode with RPL-3 CS value.  This
 * tricky test checks that with one comparison.
 *
 * On x86_64, vm86 mode is mercifully nonexistent, and we don't need
 * the extra check.
 */

static inline int user_mode(struct pt_regs *regs)
{
#ifdef CONFIG_X86_64
	return !!(regs->cs & 3);
#else
	return ((regs->cs & SEGMENT_RPL_MASK) | (regs->flags & X86_VM_MASK)) >= USER_RPL;
#endif
}

#endif /* ASSMBLER */
#endif
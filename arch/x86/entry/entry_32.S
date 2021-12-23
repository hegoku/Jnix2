#include <asm/ptrace.h>
#include <asm/segment.h>
#include <asm/trapnr.h>
#include <asm/page.h>
#include <asm/irq_vectors.h>

#define PTI_SWITCH_MASK         (1 << PAGE_SHIFT)

#define CS_FROM_ENTRY_STACK	(1 << 31)
#define CS_FROM_USER_CR3	(1 << 30)
#define CS_FROM_KERNEL		(1 << 29)
#define CS_FROM_ESPFIX		(1 << 28)

.macro ENCODE_FRAME_POINTER
	mov %esp, %ebp
	andl $0x7fffffff, %ebp
.endm

.macro FIXUP_FRAME
	/*
	 * The high bits of the CS dword (__csh) are used for CS_FROM_*.
	 * Clear them in case hardware didn't do this for us.
	 */
	andl	$0x0000ffff, 4*4(%esp)

	testl	$USER_SEGMENT_RPL_MASK, 4*4(%esp)
	jnz	.Lfrom_usermode_no_fixup_\@

	orl	$CS_FROM_KERNEL, 4*4(%esp)

	/*
	 * When we're here from kernel mode; the (exception) stack looks like:
	 *
	 *  6*4(%esp) - <previous context>
	 *  5*4(%esp) - flags
	 *  4*4(%esp) - cs
	 *  3*4(%esp) - ip
	 *  2*4(%esp) - orig_eax
	 *  1*4(%esp) - gs / function
	 *  0*4(%esp) - fs
	 *
	 * Lets build a 5 entry IRET frame after that, such that struct pt_regs
	 * is complete and in particular regs->sp is correct. This gives us
	 * the original 6 entries as gap:
	 *
	 * 14*4(%esp) - <previous context>
	 * 13*4(%esp) - gap / flags
	 * 12*4(%esp) - gap / cs
	 * 11*4(%esp) - gap / ip
	 * 10*4(%esp) - gap / orig_eax
	 *  9*4(%esp) - gap / gs / function
	 *  8*4(%esp) - gap / fs
	 *  7*4(%esp) - ss
	 *  6*4(%esp) - sp
	 *  5*4(%esp) - flags
	 *  4*4(%esp) - cs
	 *  3*4(%esp) - ip
	 *  2*4(%esp) - orig_eax
	 *  1*4(%esp) - gs / function
	 *  0*4(%esp) - fs
	 */

	pushl	%ss		# ss
	pushl	%esp		# sp (points at ss)
	addl	$7*4, (%esp)	# point sp back at the previous context
	pushl	7*4(%esp)	# flags
	pushl	7*4(%esp)	# cs
	pushl	7*4(%esp)	# ip
	pushl	7*4(%esp)	# orig_eax
	pushl	7*4(%esp)	# gs / function
	pushl	7*4(%esp)	# fs
.Lfrom_usermode_no_fixup_\@:
.endm

.macro SAVE_ALL pt_regs_ax=%eax switch_stacks=0 skip_gs=0 unwind_espfix=0
	cld
.if \skip_gs == 0
	pushl	$0
.endif
	pushl	%fs

	pushl	%eax
	movl	$(__KERNEL_PERCPU), %eax
	movl	%eax, %fs
#.if \unwind_espfix > 0
#	UNWIND_ESPFIX_STACK
#.endif
	popl	%eax

	FIXUP_FRAME
	pushl	%es
	pushl	%ds
	pushl	\pt_regs_ax
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%edx
	pushl	%ecx
	pushl	%ebx
	movl	$(__USER_DS), %edx
	movl	%edx, %ds
	movl	%edx, %es
	/* Switch to kernel stack if necessary */
#.if \switch_stacks > 0
#	SWITCH_TO_KERNEL_STACK
#.endif
.endm

.macro PARANOID_EXIT_TO_KERNEL_MODE

	/*
	 * Test if we entered the kernel with the entry-stack. Most
	 * likely we did not, because this code only runs on the
	 * return-to-kernel path.
	 */
	testl	$CS_FROM_ENTRY_STACK, PT_CS(%esp)
	jz	.Lend_\@

	/* Unlikely slow-path */

	/* Clear marker from stack-frame */
	andl	$(~CS_FROM_ENTRY_STACK), PT_CS(%esp)

	/* Copy the remaining task-stack contents to entry-stack */
	movl	%esp, %esi
	movl	cpu_tss_rw + TSS_sp0, %edi

	/* Bytes on the task-stack to ecx */
	movl	cpu_tss_rw + TSS_sp1, %ecx
	subl	%esi, %ecx

	/* Allocate stack-frame on entry-stack */
	subl	%ecx, %edi

	/*
	 * Save future stack-pointer, we must not switch until the
	 * copy is done, otherwise the NMI handler could destroy the
	 * contents of the task-stack we are about to copy.
	 */
	movl	%edi, %ebx

	/* Do the copy */
	shrl	$2, %ecx
	cld
	rep movsl

	/* Safe to switch to entry-stack now */
	movl	%ebx, %esp

	/*
	 * We came from entry-stack and need to check if we also need to
	 * switch back to user cr3.
	 */
	testl	$CS_FROM_USER_CR3, PT_CS(%esp)
	jz	.Lend_\@

	/* Clear marker from stack-frame */
	andl	$(~CS_FROM_USER_CR3), PT_CS(%esp)

	SWITCH_TO_USER_CR3 scratch_reg=%eax

.Lend_\@:
.endm

.macro SWITCH_TO_USER_CR3 scratch_reg:req
#	ALTERNATIVE "jmp .Lend_\@", "", X86_FEATURE_PTI

	movl	%cr3, \scratch_reg
	orl	$PTI_SWITCH_MASK, \scratch_reg
	movl	\scratch_reg, %cr3
.Lend_\@:
.endm

.macro RESTORE_INT_REGS
	popl	%ebx
	popl	%ecx
	popl	%edx
	popl	%esi
	popl	%edi
	popl	%ebp
	popl	%eax
.endm

.macro RESTORE_REGS pop=0
	RESTORE_INT_REGS
1:	popl	%ds
2:	popl	%es
3:	popl	%fs
	addl	$(4 + \pop), %esp	/* pop the unused "gs" slot */
	IRET_FRAME
#.pushsection .fixup, "ax"
#4:	movl	$0, (%esp)
#	jmp	1b
#5:	movl	$0, (%esp)
#	jmp	2b
#6:	movl	$0, (%esp)
#	jmp	3b
#.popsection
#	_ASM_EXTABLE(1b, 4b)
#	_ASM_EXTABLE(2b, 5b)
#	_ASM_EXTABLE(3b, 6b)
.endm

.macro IRET_FRAME
	/*
	 * We're called with %ds, %es, %fs, and %gs from the interrupted
	 * frame, so we shouldn't use them.  Also, we may be in ESPFIX
	 * mode and therefore have a nonzero SS base and an offset ESP,
	 * so any attempt to access the stack needs to use SS.  (except for
	 * accesses through %esp, which automatically use SS.)
	 */
	testl $CS_FROM_KERNEL, 1*4(%esp)
	jz .Lfinished_frame_\@

	/*
	 * Reconstruct the 3 entry IRET frame right after the (modified)
	 * regs->sp without lowering %esp in between, such that an NMI in the
	 * middle doesn't scribble our stack.
	 */
	pushl	%eax
	pushl	%ecx
	movl	5*4(%esp), %eax		# (modified) regs->sp

	movl	4*4(%esp), %ecx		# flags
	movl	%ecx, %ss:-1*4(%eax)

	movl	3*4(%esp), %ecx		# cs
	andl	$0x0000ffff, %ecx
	movl	%ecx, %ss:-2*4(%eax)

	movl	2*4(%esp), %ecx		# ip
	movl	%ecx, %ss:-3*4(%eax)

	movl	1*4(%esp), %ecx		# eax
	movl	%ecx, %ss:-4*4(%eax)

	popl	%ecx
	lea	-4*4(%eax), %esp
	popl	%eax
.Lfinished_frame_\@:
.endm

/**
 * idtentry - Macro to generate entry stubs for simple IDT entries
 * @vector:		Vector number
 * @asmsym:		ASM symbol for the entry point
 * @cfunc:		C function to be called
 * @has_error_code:	Hardware pushed error code on stack
 */
.macro idtentry vector asmsym cfunc has_error_code:req
.globl \asmsym
\asmsym:
	cld

	.if \has_error_code == 0
		pushl	$0		/* Clear the error code */
	.endif

	/* Push the C-function address into the GS slot */
	pushl	$\cfunc
	/* Invoke the common exception entry */
	jmp	handle_exception
.type \asmsym, @function;
.size \asmsym, .-\asmsym
.endm

.globl handle_exception
handle_exception:
	/* the function address is in %gs's slot on the stack */
	SAVE_ALL switch_stacks=1 skip_gs=1 unwind_espfix=1
	ENCODE_FRAME_POINTER

	movl	PT_GS(%esp), %edi		# get the function address

	/* fixup orig %eax */
	movl	PT_ORIG_EAX(%esp), %edx		# get the error code
	movl	$-1, PT_ORIG_EAX(%esp)		# no syscall to restart

	movl	%esp, %eax			# pt_regs pointer
#	CALL_NOSPEC edi
	call	*%edi

handle_exception_return:
	/*
	 * We can be coming here from child spawned by kernel_thread().
	 */
	movl	PT_CS(%esp), %eax
	andl	$SEGMENT_RPL_MASK, %eax

	cmpl	$USER_RPL, %eax			# returning to v8086 or userspace ?
	jnb	ret_to_user

#	PARANOID_EXIT_TO_KERNEL_MODE
#	BUG_IF_WRONG_CR3
	RESTORE_REGS 4
	jmp	.Lirq_return

ret_to_user:
	movl	%esp, %eax
	jmp	restore_all_switch_stack
.type handle_exception, @function;
.size handle_exception, .-handle_exception

restore_all_switch_stack:
#	SWITCH_TO_ENTRY_STACK
#	CHECK_AND_APPLY_ESPFIX

	/* Switch back to user CR3 */
	SWITCH_TO_USER_CR3 scratch_reg=%eax

#	BUG_IF_WRONG_CR3

	/* Restore user state */
	RESTORE_REGS pop=4			# skip orig_eax/error_code
.Lirq_return:
	/*
	 * ARCH_HAS_MEMBARRIER_SYNC_CORE rely on IRET core serialization
	 * when returning from IPI handler and when returning from
	 * scheduler to user-space.
	 */
	iret

.align 8
.globl irq_entries_start
irq_entries_start:
	vector=FIRST_EXTERNAL_VECTOR
    .rept NR_EXTERNAL_VECTORS
#	UNWIND_HINT_IRET_REGS
0 :
	.byte	0x6a, vector
	jmp	asm_common_interrupt
	nop
	/* Ensure that the above is 8 bytes max */
	. = 0b + 8
	vector = vector+1
    .endr
.type irq_entries_start, @function;
.size irq_entries_start, .-irq_entries_start

####
idtentry vector=X86_TRAP_PF asmsym=asm_exc_page_fault cfunc=exc_page_fault has_error_code=1

idtentry vector=0xFFFF asmsym=asm_common_interrupt cfunc=common_interrupt has_error_code=1
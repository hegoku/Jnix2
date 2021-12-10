#include <asm/ptrace.h>
#include <asm/segment.h>
#include <asm/trapnr.h>

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

#handle_exception_return:
	/*
	 * We can be coming here from child spawned by kernel_thread().
	 */
#	movl	PT_CS(%esp), %eax
#	andl	$SEGMENT_RPL_MASK, %eax

#	cmpl	$USER_RPL, %eax			# returning to v8086 or userspace ?
#	jnb	ret_to_user

#	PARANOID_EXIT_TO_KERNEL_MODE
#	BUG_IF_WRONG_CR3
#	RESTORE_REGS 4
#	jmp	.Lirq_return

#ret_to_user:
#	movl	%esp, %eax
#	jmp	restore_all_switch_stack
.type handle_exception, @function;
.size handle_exception, .-handle_exception




####
idtentry vector=X86_TRAP_PF asmsym=asm_exc_page_fault cfunc=exc_page_fault has_error_code=1
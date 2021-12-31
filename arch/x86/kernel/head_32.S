#include <uapi/asm/bootparam.h>
#include <asm/setup.h>
#include <jnix/init.h>
#include <asm/page.h>
#include <asm/segment.h>
#include <asm/thread_info.h>

#define pa(X) ((X) - __PAGE_OFFSET)
#define PARAM_SIZE_UNIT 1024 //PARAM_SIZE/4
#define SIZEOF_PTREGS 17*4

//.section ".head.text", "ax"
__HEAD
.code32
.global startup_32
startup_32:
	movl pa(initial_stack),%ecx
	lgdt pa(boot_gdt_descr)
	movl $(__BOOT_DS),%eax
	movl %eax,%ds
	movl %eax,%es
	movl %eax,%fs
	movl %eax,%gs
	movl %eax,%ss
	leal -__PAGE_OFFSET(%ecx),%esp
	/*
	* Clear BSS first so that there are no surprises...
	*/
	cld
	xorl %eax,%eax
	movl $pa(__bss_start),%edi
	movl $pa(__bss_stop),%ecx
	subl %edi,%ecx
	shrl $2,%ecx
	rep ; stosl

	movl $pa(boot_params),%edi
	movl $(PARAM_SIZE_UNIT),%ecx
	cld
	rep
	movsl
	call mk_early_pgtbl_32
	
.Lenable_paging:
	movl $pa(swapper_pg_dir),%eax
	movl %eax,%cr3
	movl %cr0,%eax
	orl $0x80000000,%eax
	movl %eax,%cr0
	ljmp $__BOOT_CS,$1f	/* Clear prefetch and normalize %eip */
1:
	/* Shift the stack pointer to a virtual address */
	addl $__PAGE_OFFSET, %esp
	lgdt early_gdt_descr
	ljmp $(__KERNEL_CS),$1f
1:	movl $(__KERNEL_DS),%eax	# reload all the segment registers
	movl %eax,%ss			# after changing gdt.

	movl $(__USER_DS),%eax		# DS/ES contains default USER segment
	movl %eax,%ds
	movl %eax,%es

	movl $(__KERNEL_PERCPU), %eax
	movl %eax,%fs			# set this cpu's percpu

	movl $(__KERNEL_STACK_CANARY),%eax
	movl %eax,%gs

	movw $(__KERNEL_TSS),%ax # load TSS
	ltr %ax

	xorl %eax,%eax			# Clear LDT
	lldt %ax

	call *(initial_code)
1:	jmp 1b

__INIT

.globl early_idt_handler_array
early_idt_handler_array:
	# 36(%esp) %eflags
	# 32(%esp) %cs
	# 28(%esp) %eip
	# 24(%rsp) error code
	i = 0
	.rept NUM_EXCEPTION_VECTORS
	.if ((EXCEPTION_ERRCODE_MASK >> i) & 1) == 0
	pushl $0		# Dummy error code, to make stack frame uniform
	.endif
	pushl $i		# 20(%esp) Vector number
	jmp early_idt_handler_common
	i = i + 1
	.fill early_idt_handler_array + i*EARLY_IDT_HANDLER_SIZE - ., 1, 0xcc
	.endr
.type early_idt_handler_array, @function;
.size early_idt_handler_array, .-early_idt_handler_array

.globl early_idt_handler_common
early_idt_handler_common:
/*
	 * The stack is the hardware frame, an error code or zero, and the
	 * vector number.
	 */
	cld
	incl %ss:early_recursion_flag

	/* The vector number is in pt_regs->gs */

	cld
	pushl	%fs		/* pt_regs->fs (__fsh varies by model) */
	pushl	%es		/* pt_regs->es (__esh varies by model) */
	pushl	%ds		/* pt_regs->ds (__dsh varies by model) */
	pushl	%eax		/* pt_regs->ax */
	pushl	%ebp		/* pt_regs->bp */
	pushl	%edi		/* pt_regs->di */
	pushl	%esi		/* pt_regs->si */
	pushl	%edx		/* pt_regs->dx */
	pushl	%ecx		/* pt_regs->cx */
	pushl	%ebx		/* pt_regs->bx */

	/* Fix up DS and ES */
	movl	$(__KERNEL_DS), %ecx
	movl	%ecx, %ds
	movl	%ecx, %es

	/* Load the vector number into EDX */
	movl	40(%esp), %edx

	/* Load GS into pt_regs->gs (and maybe clobber __gsh) */
	movw	%gs, 40(%esp)

	movl	%esp, %eax	/* args are pt_regs (EAX), trapnr (EDX) */
	call	early_fixup_exception
	popl	%ebx		/* pt_regs->bx */
	popl	%ecx		/* pt_regs->cx */
	popl	%edx		/* pt_regs->dx */
	popl	%esi		/* pt_regs->si */
	popl	%edi		/* pt_regs->di */
	popl	%ebp		/* pt_regs->bp */
	popl	%eax		/* pt_regs->ax */
	popl	%ds		/* pt_regs->ds (always ignores __dsh) */
	popl	%es		/* pt_regs->es (always ignores __esh) */
	popl	%fs		/* pt_regs->fs (always ignores __fsh) */
	popl	%gs		/* pt_regs->gs (always ignores __gsh) */
	decl	%ss:early_recursion_flag
	addl	$4, %esp	/* pop pt_regs->orig_ax */
	iret
.type early_idt_handler_common, @function;
.size early_idt_handler_common, .-early_idt_handler_common

/* This is the default interrupt "handler" :-) */
.globl early_ignore_irq
early_ignore_irq:
	cld
	iret

hlt_loop:
	hlt
	jmp hlt_loop
.type early_ignore_irq, @function;
.size early_ignore_irq, .-early_ignore_irq

__INITDATA
	.align 4
.globl early_recursion_flag
early_recursion_flag:
	.long 0
.type early_recursion_flag, STT_OBJECT;
.size early_recursion_flag, .-early_recursion_flag

__REFDATA
	.align 4
.globl initial_code
initial_code:
	.long i386_start_kernel
.type initial_code, STT_OBJECT;
.size initial_code, .-initial_code

#define	PGD_ALIGN	(PAGE_SIZE)

//__PAGE_ALIGNED_BSS
.section ".bss..page_aligned", "aw"
	.align PGD_ALIGN
.globl swapper_pg_dir
swapper_pg_dir:
	.fill 1024,4,0
.type swapper_pg_dir STT_OBJECT;
.size swapper_pg_dir, .-swapper_pg_dir

.data
.balign 4
/*
 * The SIZEOF_PTREGS gap is a convention which helps the in-kernel unwinder
 * reliably detect the end of the stack.
 */
.globl initial_stack
initial_stack:
	.long init_thread_union + THREAD_SIZE - SIZEOF_PTREGS - TOP_OF_KERNEL_STACK_PADDING
	#.long init_thread_union
.type initial_stack STT_OBJECT;
.size initial_stack, .-initial_stack

.section ".data..init_task", "a"
init_thread_union:
	.fill 1024,8,0
.type init_thread_union STT_OBJECT;
.size init_thread_union, .-init_thread_union

.data
# early boot GDT descriptor (must use 1:1 address mapping)
	.word 0				# 32 bit align gdt_desc.address
.globl boot_gdt_descr
boot_gdt_descr:
	.word __BOOT_DS+7
	.long boot_gdt - __PAGE_OFFSET
.type boot_gdt_descr STT_OBJECT;
.size boot_gdt_descr, .-boot_gdt_descr

# boot GDT descriptor (later on used by CPU#0):
	.word 0				# 32 bit align gdt_desc.address
.globl early_gdt_descr
early_gdt_descr:
	.word GDT_ENTRIES*8-1
	.long gdt_page			/* Overwritten for secondary CPUs */
.type early_gdt_descr STT_OBJECT;
.size early_gdt_descr, .-early_gdt_descr

/*
 * The boot_gdt must mirror the equivalent in setup.S and is
 * used only for booting.
 */
.globl boot_gdt
boot_gdt:
	.fill GDT_ENTRY_BOOT_CS,8,0
	.quad 0x00cf9a000000ffff	/* kernel 4GB code at 0x00000000 */
	.quad 0x00cf92000000ffff	/* kernel 4GB data at 0x00000000 */
.type boot_gdt STT_OBJECT;
.size boot_gdt, .-boot_gdt
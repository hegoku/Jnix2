#include "boot.h"
#include "uapi/asm/bootparam.h"

	.code16
	.section ".bstext", "ax"
	.section ".bsdata", "a"
	.fill 494,1,0

	.section ".header", "a"
	.globl	sentinel
sentinel:	.byte 0xff, 0xff        /* Used to detect broken loaders */

	.globl	hdr
hdr:
setup_sects:	.byte 0			/* Filled in by build.c */
root_flags:	.word 0
syssize:	.long 0			/* Filled in by build.c */
ram_size:	.word 0			/* Obsolete */
vid_mode:	.word 0
root_dev:	.word 0			/* Filled in by build.c */
boot_flag:	.word 0xAA55

.globl	_start
_start:
	# Explicitly enter this as bytes, or the assembler
	# tries to generate a 3-byte jump here, which causes
	# everything else to push off to the wrong offset.
	.byte	0xeb		# short (2-byte) jump
	.byte	start_of_setup-1f
1:
	header: .ascii "HdrS"
	version: .word 0x205
	realmode_swtch: .long 0
	start_sys_seg: .word 0x1000
	kernel_version: .word 0
	type_of_loader: .byte 0
	loadflags: .byte LOADED_HIGH
	setup_move_size: .word 0
	code32_start: .long 0x100000
	ramdisk_image: .long 0
	ramdisk_size: .long 0
	bootsect_kludge: .long 0
	heap_end_ptr: .long 0x0200
	ext_loader_ver: .byte 0
	ext_loader_type: .byte 0
	cmd_line_ptr: .long 0
	initrd_addr_max: .long 0x37FFFFFF


.section ".entrytext", "ax"
start_of_setup:
	movw	%ds, %ax
	movw	%ax, %es
	cld

# Apparently some ancient versions of LILO invoked the kernel with %ss != %ds,
# which happened to work by accident for the old code.  Recalculate the stack
# pointer if %ss is invalid.  Otherwise leave it alone, LOADLIN sets up the
# stack behind its own code, so we can't blindly put it directly past the heap.

	movw	%ss, %dx
	cmpw	%ax, %dx	# %ds == %ss?
	movw	%sp, %dx
	je	2f		# -> assume %sp is reasonably set

	# Invalid %ss, make up a new stack
	movw	$_end, %dx
	testb	$CAN_USE_HEAP, loadflags
	jz	1f
	movw	heap_end_ptr, %dx
1:	addw	$STACK_SIZE, %dx
	jnc	2f
	xorw	%dx, %dx	# Prevent wraparound

2:	# Now %dx should point to the end of our stack space
	andw	$~3, %dx	# dword align (might as well...)
	jnz	3f
	movw	$0xfffc, %dx	# Make sure we're not zero
3:	movw	%ax, %ss
	movzwl	%dx, %esp	# Clear upper half of %esp
	sti			# Now we should have a working stack

# We will have entered with %cs = %ds+0x20, normalize %cs so
# it is on par with the other segments.
	pushw	%ds
	pushw	$6f
	lretw
6:
# Zero the bss
	movw	$__bss_start, %di
	movw	$_end+3, %cx
	xorl	%eax, %eax
	subw	%di, %cx
	shrw	$2, %cx
	rep; stosl
	
# Jump to C code (should not return)
	calll	main
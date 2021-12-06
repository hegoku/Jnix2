#ifndef END_FUNC
#define END_FUNC(name)				\
	.type name, @function ;			\
	.size name, .-name
#endif

#ifndef START_FUNC
#define START_FUNC(name)			\
	.globl name				;\
	name:
#endif

.code16
	.text

START_FUNC(memcpy)
	pushw	%si
	pushw	%di
	movw	%ax, %di
	movw	%dx, %si
	pushw	%cx
	shrw	$2, %cx
	rep; movsl
	popw	%cx
	andw	$3, %cx
	rep; movsb
	popw	%di
	popw	%si
	retl
END_FUNC(memcpy)

START_FUNC(memset)
	pushw	%di
	movw	%ax, %di
	movzbl	%dl, %eax
	imull	$0x01010101,%eax
	pushw	%cx
	shrw	$2, %cx
	rep; stosl
	popw	%cx
	andw	$3, %cx
	rep; stosb
	popw	%di
	retl
END_FUNC(memset)

START_FUNC(copy_from_fs)
	pushw	%ds
	pushw	%fs
	popw	%ds
	calll	memcpy
	popw	%ds
	retl
END_FUNC(copy_from_fs)

START_FUNC(copy_to_fs)
	pushw	%es
	pushw	%fs
	popw	%es
	calll	memcpy
	popw	%es
	retl
END_FUNC(copy_to_fs)
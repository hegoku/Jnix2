#include "boot.h"
#include "string.h"
#include "video.h"

struct boot_params boot_params __attribute__((aligned(16)));

char *HEAP = _end;
char *heap_end = _end;		/* Default end of heap = no heap */

static void copy_boot_params(void)
{
	// struct old_cmdline {
	// 	u16 cl_magic;
	// 	u16 cl_offset;
	// };
	// const struct old_cmdline * const oldcmd =
	// 	(const struct old_cmdline *)OLD_CL_ADDRESS;

	// BUILD_BUG_ON(sizeof(boot_params) != 4096);
	memcpy(&boot_params.hdr, &hdr, sizeof(hdr));

	// if (!boot_params.hdr.cmd_line_ptr &&
	//     oldcmd->cl_magic == OLD_CL_MAGIC) {
	// 	/* Old-style command line protocol. */
	// 	u16 cmdline_seg;

	// 	/* Figure out if the command line falls in the region
	// 	   of memory that an old kernel would have copied up
	// 	   to 0x90000... */
	// 	if (oldcmd->cl_offset < boot_params.hdr.setup_move_size)
	// 		cmdline_seg = ds();
	// 	else
	// 		cmdline_seg = 0x9000;

	// 	boot_params.hdr.cmd_line_ptr =
	// 		(cmdline_seg << 4) + oldcmd->cl_offset;
	// }
}

static void init_heap(void)
{
	char *stack_end;

	if (boot_params.hdr.loadflags & CAN_USE_HEAP) {
		asm("leal %P1(%%esp),%0"
		    : "=r" (stack_end) : "i" (-STACK_SIZE));

		heap_end = (char *)
			((size_t)boot_params.hdr.heap_end_ptr + 0x200);
		if (heap_end > stack_end)
			heap_end = stack_end;
	} else {
		/* Boot protocol 2.00 only, no heap available */
		puts("WARNING: Ancient bootloader, some functionality "
		     "may be limited!\n");
	}
}

void set_video(void)
{
	// u16 mode = boot_params.hdr.vid_mode;

	RESET_HEAP();

	// store_mode_params();
	// save_screen();
	probe_cards(0);

	// for (;;) {
	// 	if (mode == ASK_VGA)
	// 		mode = mode_menu();

	// 	if (!set_mode(mode))
	// 		break;

	// 	printf("Undefined video mode number: %x\n", mode);
	// 	mode = ASK_VGA;
	// }
	// set_mode(mode);
	set_mode(VIDEO_80x25);
	// boot_params.hdr.vid_mode = mode;
	// store_mode_params();

	// if (do_restore)
	// 	restore_screen();
}

void main(void)
{
	copy_boot_params();
	init_heap();
	detect_memory();
	set_video();

	// printf("setup_sects: %d, type_of_loader: %x, code32_start: %x, heap_end_ptr: %x\ncmd_line_ptr: %x, loadflags: %x, ", boot_params.hdr.setup_sects, boot_params.hdr.type_of_loader,
	// 	boot_params.hdr.code32_start, boot_params.hdr.heap_end_ptr, boot_params.hdr.cmd_line_ptr, boot_params.hdr.loadflags
	// );
	// printf("vid_mode: %x\n", boot_params.hdr.vid_mode);

	go_to_protected_mode();
}
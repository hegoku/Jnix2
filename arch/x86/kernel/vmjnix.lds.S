
PAGE_SIZE = 4096;
LOAD_PHYSICAL_ADDR = 0x100000;
LOAD_OFFSET = 0xC0000000;

THREAD_SIZE = 1024*8;

OUTPUT_ARCH(i386)
ENTRY(phys_startup_32)

PHDRS {
	text PT_LOAD FLAGS(5);          /* R_E */
	data PT_LOAD FLAGS(6);          /* RW_ */
	note PT_NOTE FLAGS(0);          /* ___ */
}

SECTIONS
{
	. = LOAD_OFFSET + LOAD_PHYSICAL_ADDR;
	phys_startup_32 = ABSOLUTE(startup_32 - LOAD_OFFSET);
	.text : AT(ADDR(.text) - LOAD_OFFSET) { 
		_text = .;
		_stext = .;
		
		KEEP(*(.head.text))
		. = ALIGN(8);

		*(.text)
	} :text =0xcccc
	_etext = .;
	. = ALIGN(PAGE_SIZE);

	/*RO_DATA*/
	. = ALIGN((PAGE_SIZE));
	.rodata : AT(ADDR(.rodata) - LOAD_OFFSET) {
		__start_rodata = .;
		*(.rodata) *(.rodata.*)
		/*SCHED_DATA*/
		/*RO_AFTER_INIT_DATA	 Read only after init */
		. = ALIGN(8);
		__start_ro_after_init = .;
		*(.data..ro_after_init)
		/*JUMP_TABLE_DATA							\*/
		/*STATIC_CALL_DATA*/
		__end_ro_after_init = .;
		. = ALIGN(8);

		__start___tracepoints_ptrs = .;
		KEEP(*(__tracepoints_ptrs)) /* Tracepoints: pointer array */
		__stop___tracepoints_ptrs = .;
		*(__tracepoints_strings)/* Tracepoints: strings */
	}
	. = ALIGN(PAGE_SIZE);
	__end_rodata_aligned = .;


	.data : AT(ADDR(.data) - LOAD_OFFSET){
		_sdata = .;

		/* init_task 
		INIT_TASK_DATA(THREAD_SIZE)*/
		. = ALIGN(THREAD_SIZE);
		__start_init_task = .;
		init_thread_union = .;
		init_stack = .;	
		KEEP(*(.data..init_task))
		KEEP(*(.data..init_thread_info))
		. = __start_init_task + THREAD_SIZE;
		__end_init_task = .;

		. = ALIGN(PAGE_SIZE);
		*(.data..page_aligned)
		. = ALIGN(PAGE_SIZE);

		/*DATA_DATA*/
		*(.data)
		*(.ref.data)

		_edata = .;
	} :data

	/* Init code and data - will be freed after init */
	. = ALIGN(PAGE_SIZE);
	.init.begin : AT(ADDR(.init.begin) - LOAD_OFFSET) {
		__init_begin = .; /* paired with __init_end */
	}

	/*INIT_TEXT_SECTION(PAGE_SIZE)*/
	. = ALIGN(PAGE_SIZE);
	.init.text : AT(ADDR(.init.text) - LOAD_OFFSET) {
		_sinittext = .;
		*(.init.text .init.text.*)
		*(.text.startup)
		*(.meminit.text*)
		_einittext = .;
	}

	/*INIT_DATA_SECTION(16)*/
	.init.data : AT(ADDR(.init.data) - LOAD_OFFSET) {
		/*INIT_DATA*/
		*(.init.data init.data.*)
		
		/*INIT_SETUP(16)*/
		. = ALIGN(16);
		__setup_start = .;
		KEEP(*(.init.setup))
		__setup_end = .;

		/*INIT_CALLS*/
		__initcall_start = .;
		KEEP(*(.initcallearly.init))
		__initcall0_start = .;
		KEEP(*(.initcall0.init))
		KEEP(*(.initcall0s.init))
		__initcall1_start = .;
		KEEP(*(.initcall1.init))
		KEEP(*(.initcall1s.init))
		__initcall2_start = .;
		KEEP(*(.initcall2.init))
		KEEP(*(.initcall2s.init))
		__initcall3_start = .;
		KEEP(*(.initcall3.init))
		KEEP(*(.initcall3s.init))
		__initcall4_start = .;
		KEEP(*(.initcall4.init))
		KEEP(*(.initcall4s.init))
		__initcall5_start = .;
		KEEP(*(.initcall5.init))
		KEEP(*(.initcall5s.init))
		__initcallrootfs_start = .;
		KEEP(*(.initcallrootfs.init))
        KEEP(*(.initcallrootfss.init))
        __initcall6_start = .;
        KEEP(*(.initcall6.init))
        KEEP(*(.initcall6s.init))
        __initcall7_start = .;
        KEEP(*(.initcall7.init))
        KEEP(*(.initcall7s.init))
		__initcall_end = .;

		/*CON_INITCALL*/
		__con_initcall_start = .;
		KEEP(*(.con_initcall.init))
		__con_initcall_end = .;

		/*INIT_RAM_FS*/
		. = ALIGN(4);
		__initramfs_start = .;
		KEEP(*(.init.ramfs))
		. = ALIGN(8);
		KEEP(*(.init.ramfs.info))
	}

	/* freed after init ends here */
	.init.end : AT(ADDR(.init.end) - LOAD_OFFSET) {
		__init_end = .;
	}

	.bss : AT(ADDR(.bss) - LOAD_OFFSET) { 
		__bss_start = .;
		*(.bss..page_aligned)
		. = ALIGN(PAGE_SIZE);
		*(.bss)
		__bss_stop = .;
	}

	__end_of_kernel_reserve = .;

	. = ALIGN(PAGE_SIZE);
	.brk : AT(ADDR(.brk) - LOAD_OFFSET) {
		__brk_base = .;
		. += 64 * 1024;		/* 64k alignment slop space */
		*(.brk_reservation)	/* areas brk users have reserved */
		__brk_limit = .;
	}

	. = ALIGN(PAGE_SIZE);		/* keep VO_INIT_SIZE page aligned */
	_end = .;
}